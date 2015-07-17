
#include "oserver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/signalfd.h>

#define DAEMON_NAME "opend"
#define MAINSOCK_ADDR "/tmp/" DAEMON_NAME ".socket"
#define LOCKFILE "/var/run/" DAEMON_NAME ".pid"
#define LOCKFILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
#define BACKLOG_SZ 10

static int sockfd;
static int epoll_fd;
static int sig_fd;
static int lockfile;

void sighup_rehash(void) {
	syslog(LOG_INFO, "Got SIGHUP, rehashing...\n");
	/* This is just a stub. Rehashing logic could be implemented here.
	 * Note that this function is not being executed as a signal handler because we use
	 * signalfd(2) to process signals synchronously. Thus, it is safe for this function
	 * to call other functions that are not async-signal safe.
	 */
}

/* Terminates the server */
void leave(void) {
	unlink(MAINSOCK_ADDR);
	exit(EXIT_SUCCESS);
}

/* Returns 0 if the process identified by the credentials in `who` *CANNOT* access the file
 * in `path`; otherwise returns non-zero
 */
int can_access(const char *path, const struct ucred *who) {
	/* This is just a stub. We could now create custom permission rules and enforce them
	 * using this function.
	 */
	return 1;
}

int already_running(void) {
	static char buff[32];

	lockfile = open(LOCKFILE, O_RDWR | O_CREAT, LOCKFILEMODE);
	if (lockfile < 0) {
		return -1;
	}

	struct flock flock;
	flock.l_type = F_WRLCK;
	flock.l_whence = SEEK_SET;
	flock.l_start = 0;
	flock.l_len = 0;

	if (fcntl(lockfile, F_SETLK, &flock) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			return 1;
		} else {
			goto errout;
		}
	}

	if (ftruncate(lockfile, 0) < 0) {
		syslog(LOG_INFO, "Error truncating lockfile: %s\n", strerror(errno));
		goto errout;
	}

	int to_write = sprintf(buff, "%ld\n", (long) getpid());
	ssize_t written;
	if ((written = write(lockfile, buff, to_write)) != to_write) {
		if (written >= 0) {
			errno = EIO;
		}
		goto errout;
	}

	return 0;

errout:
	;
	int saved = errno;
	/* close(2) may change errno if it fails */
	close(lockfile);
	errno = saved;
	return -1;
}


void daemonize(void) {

	umask(0);

	if (chdir("/") < 0) {
		perror("Fatal: chdir(2) failed");
		exit(EXIT_FAILURE);
	}

	openlog(DAEMON_NAME, 0, 0);

	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("Fatal: fork(2) failed");
		exit(EXIT_FAILURE);
	}

	if (pid != 0) {
		exit(EXIT_SUCCESS);
	}

	setsid();

	if ((pid = fork()) < 0) {
		perror("Fatal: Couldn't fork after creating session");
		exit(EXIT_FAILURE);
	}

	if (pid != 0) {
		exit(EXIT_SUCCESS);
	}

	/* We can only do this here because already_running() atomically checks for other instances
	 * and acquires a write lock on the PID file. This must be the last step after all forks are
	 * done, because locks are lost when a process dies (and they are obviously not inherited by
	 * the child processes)
	 *
	 * We also want to report any errors about duplicate instances to stderr to give the user a
	 * chance to realize the mistake, instead of silently writing to syslog. Thus, we need to do
	 * it before closing stderr.
	 *
	 */
	int check;
	if ((check = already_running()) == 1) {
		fprintf(stderr, "Another instance of " DAEMON_NAME " was detected in the system.\n");
		fprintf(stderr, "See " LOCKFILE " to get its PID.\n");
		fprintf(stderr, "Terminating...\n");
		exit(EXIT_SUCCESS);
	} else if (check == -1) {
		fprintf(stderr, "Fatal: Error checking for other instances: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int sink = open("/dev/null", O_RDWR, 0);
	if (sink < 0) {
		fprintf(stderr, "Fatal: Couldn't open /dev/null: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (dup2(sink, STDIN_FILENO) < 0) {
		fprintf(stderr, "Fatal: Couldn't dup() /dev/null into stdin: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (dup2(sink, STDOUT_FILENO) < 0) {
		fprintf(stderr, "Fatal: Couldn't dup() /dev/null into stdout: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (dup2(sink, STDERR_FILENO) < 0) {
		fprintf(stderr, "Couldn't dup() /dev/null into stderr: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (sink != STDIN_FILENO && sink != STDOUT_FILENO && sink != STDERR_FILENO) {
		close(sink);
	}
}

void prepare_loop(void) {

	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGINT);
	sigaddset(&sigmask, SIGTERM);
	sigaddset(&sigmask, SIGHUP);

	if (sigprocmask(SIG_BLOCK, &sigmask, NULL) < 0) {
		syslog(LOG_ERR, "Unable to block signals: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((sig_fd = signalfd(-1, &sigmask, 0)) < 0) {
		syslog(LOG_ERR, "signalfd(2) error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((epoll_fd = epoll_create(1)) < 0) {
		syslog(LOG_ERR, "Couldn't create epoll fd: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct epoll_event ep_event;
	ep_event.events = EPOLLIN | EPOLLET;
	ep_event.data.fd = sockfd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ep_event) < 0) {
		syslog(LOG_ERR, "epoll_ctl(2) error when trying to add main socket: %s\n",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	ep_event.data.fd = sig_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sig_fd, &ep_event) < 0) {
		syslog(LOG_ERR, "epoll_ctl(2) error when trying to add signal fd: %s\n",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void serve_client_request(int clientfd, char *request, const struct ucred *src) {
	char *buff;
	int omode;

	if ((buff = strdup(request)) == NULL) {
		syslog(LOG_ERR, "strdup(3) couldn't allocate memory");
		return;
	}

	if (sscanf(request, "open %s %d", buff, &omode) != 2) {
		syslog(LOG_ERR, "Invalid request: %s\n", request);
		goto outbuff;
	}

	if (!can_access(buff, src)) {
		errno = EACCES;
		if (send_errno(clientfd) < 0) {
			syslog(LOG_ERR, "send_errno() failed attempting to notify client: %s\n",
			       strerror(errno));
			syslog(LOG_ERR, "Note: Permission denied for pid %ld to open %s (euid = %ld, egid = %ld)\n",
			       (long) src->pid, buff, (long) src->uid, (long) src->gid);
		}
		goto outbuff;
	}

	int fd = open(buff, omode);

	if (fd < 0) {
		int open_error = errno;
		if (send_errno(clientfd) < 0) {
			syslog(LOG_ERR, "send_errno() failed attempting to notify client: %s\n",
			       strerror(errno));
			syslog(LOG_ERR, "Note: couldn't open %s: %s\n", buff, strerror(open_error));
		}
		goto outbuff;
	}
	if (send_fd(clientfd, fd) < 0) {
		syslog(LOG_ERR, "send_fd() failed attempting to send fd for %s: %s\n",
		       buff, strerror(errno));
		goto outclose;
	}

outclose:
	close(fd);
outbuff:
	free(buff);
}

void handle_client_request(int clientfd) {
	static char req_buff[2048];

	struct iovec iov;
	iov.iov_base = req_buff;
	iov.iov_len = sizeof(req_buff)-1;

	unsigned char cmsg_buff[CMSG_SPACE(sizeof(struct ucred))];

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = sizeof(cmsg_buff);
	msg.msg_flags = 0;

	ssize_t n;
	n = recvmsg(clientfd, &msg, 0);

	if (n == 0) {
		syslog(LOG_INFO, "Ignoring empty message from client.\n");
		return;
	} else if (n < 0) {
		syslog(LOG_ERR, "Error retrieving message from client: %s\n", strerror(errno));
		return;
	}

	if (msg.msg_controllen != sizeof(cmsg_buff)) {
		errno = EINVAL;
		if (send_errno(clientfd) < 0) {
			syslog(LOG_ERR, "send_errno() failed when attempting to notify client of invalid or non-existent cmsg: %s\n",
			       strerror(errno));
		}
	} else {
		struct ucred *ucredptr;
		ucredptr = (struct ucred *) CMSG_DATA((struct cmsghdr *) cmsg_buff);
		serve_client_request(clientfd, req_buff, ucredptr);
	}
}

void handle_event(const struct epoll_event *event) {
	struct signalfd_siginfo signalinfo;

	if (event->data.fd == sig_fd) {
		ssize_t n;
		if ((n = read(event->data.fd, &signalinfo, sizeof(signalinfo))) < 0) {
			syslog(LOG_ERR, "Error reading from signal fd: %s\n", strerror(errno));
			return;
		}
		if (n < sizeof(signalinfo)) {
			syslog(LOG_ERR, "Partial read(2) on signal fd detected (%zu/%zu)\n", n, sizeof(signalinfo));
			return;
		}
		if (signalinfo.ssi_signo == SIGHUP) {
			sighup_rehash();
			return;
		} else if (signalinfo.ssi_signo == SIGTERM) {
			syslog(LOG_INFO, "Got SIGTERM, exiting...\n");
			leave();
		} else if (signalinfo.ssi_signo == SIGINT) {
			syslog(LOG_INFO, "Got SIGINT, exiting...\n");
			leave();
		} else {
			syslog(LOG_INFO, "Unexpected signal received: %s\n", strsignal(signalinfo.ssi_signo));
		}
	}

	if (event->data.fd == sockfd) {
		int clientfd;
		if ((clientfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "Error accepting new client: %s\n", strerror(errno));
			return;
		}

		int optval = 1;
		if (setsockopt(clientfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0) {
			syslog(LOG_ERR, "Error setting SO_PASSCRED on new client: %s\n", strerror(errno));
			close(clientfd);
			return;
		}

		struct epoll_event event;
		event.events = EPOLLIN | EPOLLET;
		event.data.fd = clientfd;

		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientfd, &event) < 0) {
			syslog(LOG_ERR, "Error adding new client to epoll set: %s\n", strerror(errno));
			close(clientfd);
		}

		return;
	}

	handle_client_request(event->data.fd);
}

void main_loop(void) {
	static struct epoll_event ep_events[1024];

	syslog(LOG_INFO, "Initialization successful. Entering main loop.\n");

	while (1) {
		int ev_count = epoll_wait(epoll_fd, ep_events, sizeof(ep_events), -1);
		if (ev_count < 0) {
			syslog(LOG_ERR, "Fatal: epoll_wait(2) error: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		int i;
		for (i = 0; i < ev_count; i++) {
			/* The core */
			if (ep_events[i].events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)) {
				if (ep_events[i].data.fd == sockfd) {
					syslog(LOG_ERR, "Fatal: unknown error in main socket.\n");
					exit(EXIT_FAILURE);
				} else if (ep_events[i].data.fd == sig_fd) {
					syslog(LOG_ERR, "Fatal: unknown error in signal fd.\n");
					exit(EXIT_FAILURE);
				}

				int res;
				res = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);

				if (res < 0) {
					if (errno == ENOENT) {
						/* Should never happen */
						syslog(LOG_ERR, "Attempted to delete client that doesn't exist..?!");
					} else {
						syslog(LOG_ERR, "Fatal: error deleting client: %s\n", strerror(errno));
						exit(EXIT_FAILURE);
					}
				}
			} else if (ep_events[i].events & EPOLLIN) {
				handle_event(&ep_events[i]);
			} else {
				syslog(LOG_ERR, "Fatal: unknown events reported by epoll_wait(2)\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

int main(int argc, char *argv[]) {
	printf(DAEMON_NAME " booting, dropping to background...\n");
	daemonize();

	struct sockaddr_un sockaddr;
	sockaddr.sun_family = AF_UNIX;
	if (sizeof(MAINSOCK_ADDR) > sizeof(sockaddr.sun_path)) {
		syslog(LOG_ERR, "Socket pathname is too long (size is %zu, max. allowed is %zu\n",
		       sizeof(MAINSOCK_ADDR)-1, sizeof(sockaddr.sun_path)-1);
		exit(EXIT_FAILURE);
	}
	strcpy(sockaddr.sun_path, MAINSOCK_ADDR);
	socklen_t sockaddr_sz = sizeof(sockaddr)-sizeof(sockaddr.sun_path)+sizeof(MAINSOCK_ADDR);

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		syslog(LOG_ERR, "socket(2) error when trying to create main socket: %s\n",
		       strerror(errno));
		exit(EXIT_FAILURE);
	}

	unlink(MAINSOCK_ADDR); /* In case it exists from previous executions */

	if (bind(sockfd, (struct sockaddr *) &sockaddr, sockaddr_sz) < 0) {
		syslog(LOG_ERR, "bind(2) error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, BACKLOG_SZ) < 0) {
		syslog(LOG_ERR, "listen(2) error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	prepare_loop();
	main_loop();

	return 0;
}
