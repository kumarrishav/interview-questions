
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define LISTEN_PORT 3000
#define BACKLOG_SZ 10
#define PTY_PATH "/home/filipe/dev/interview-questions/knowledge_based/linux_internals/pseudo_terminals/pty"
#define LOGIN_PATH "/bin/login"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * The daemonize library. See knowledge_based/linux_internals/daemonize_unique for more information
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#define DAEMON_NAME "remotelogind"
#define LOCKFILE "/var/run/" DAEMON_NAME ".pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void sighup_handler(int signo) {
	syslog(LOG_INFO, "Got SIGHUP, rehashing...\n");
	/* ... */
}

void terminate_handler(int signo) {
	syslog(LOG_INFO, "Terminating...\n");
	exit(EXIT_SUCCESS);
}

int already_running(void) {
	static char pid_buff[32];

	int lockfile = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);
	if (lockfile < 0) {
		return -1;
	}

	/* Attempt to lock the entire file */
	struct flock flock;
	flock.l_type = F_WRLCK;
	flock.l_whence = SEEK_SET;
	flock.l_start = 0;
	flock.l_len = 0;

	if (fcntl(lockfile, F_SETLK, &flock) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			// Already running
			return 1;
		}

		// Some other error
		return -1;
	}

	if (ftruncate(lockfile, 0) < 0) {
		return -1;
	}

	int len = snprintf(pid_buff, sizeof(pid_buff), "%ld\n", (long) getpid());

	if (write(lockfile, pid_buff, len) != len) {
		return -1;
	}

	return 0;
}

int daemonize(void) {

	umask(0);

	pid_t pid;
	pid = fork();

	if (pid < 0) {
		return -1;
	}

	/* Ensure that we're not a process group leader,
	 * so that it's safe to create a new session
	 */
	if (pid != 0) {
		exit(0);
	}

	setsid();

	/* Close every open file descriptor */
	struct rlimit rlim;
	int max_fd;
	if (getrlimit(RLIMIT_NOFILE, &rlim) < 0 || rlim.rlim_cur == RLIM_INFINITY) {
		max_fd = 1024;
	} else {
		max_fd = rlim.rlim_cur;
	}

	int i;
	for (i = 0; i < max_fd; i++) {
		close(i);
	}

	/* chdir to root, or to a specified, known location where the daemon
	 * will do its work
	 */
	chdir("/");

	/* Prepare error / message logging */
	openlog(DAEMON_NAME, 0, 0);

	/* Ensure that we're not a session leader to prevent controlling terminal allocation
	 * This is recommended on System V based systems, but not really needed on other UNIX
	 * flavors
	 */
	pid = fork();
	if (pid < 0) {
		syslog(LOG_ERR, "Couldn't fork() after creating a new session.\n");
		exit(EXIT_FAILURE);
	}
	if (pid != 0) {
		exit(0);
	}

	/* Some library functions and system utilities expect to have stdin, stdout and stderr
	 * opened */
	int sink = open("/dev/null", O_RDWR, 0);
	dup2(sink, STDIN_FILENO);

	if (sink != STDIN_FILENO) {
		close(sink);
	}

	dup2(STDIN_FILENO, STDOUT_FILENO);
	dup2(STDIN_FILENO, STDERR_FILENO);

	/* Catch SIGHUP. SIGHUP on daemons is typically used to force the daemon to reread its
	 * configuration file.
	 * Why SIGHUP? Because SIGHUP is the signal sent by the terminal driver to the controlling
	 * process in a session when the terminal driver detects a network disconnect or a modem
	 * hangup (hence "HUP").
	 * Since daemons do not have a controlling terminal, SIGHUP can't possibly be triggered
	 * by a terminal disconnect. Thus, people have historically used it with a different meaning
	 * in daemons
	 */

	struct sigaction sighandler;
	sighandler.sa_handler = sighup_handler;
	sighandler.sa_flags = 0;
	sigemptyset(&sighandler.sa_mask);

	if (sigaction(SIGHUP, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGHUP.\n");
		exit(EXIT_FAILURE);
	}

	sighandler.sa_handler = terminate_handler;
	if (sigaction(SIGINT, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGINT.\n");
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sighandler, NULL) < 0) {
		syslog(LOG_ERR, "Couldn't catch SIGTERM.\n");
		exit(EXIT_FAILURE);
	}

	int running = already_running();
	if (running == -1) {
		syslog(LOG_INFO, "already_running() error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	} else if (running == 1) {
		syslog(LOG_INFO, "Daemon is already running. See " LOCKFILE "\n");
		exit(EXIT_SUCCESS);
	}

	syslog(LOG_INFO, "daemonize() complete. PID = %ld\n", (long) getpid());
	syslog(LOG_INFO, "Send SIGTERM or SIGINT to terminate\n");

	return 0;
}

int accept_client(int clientfd);

int main(void) {

	if (daemonize() < 0) {
		exit(EXIT_FAILURE);
	}

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		syslog(LOG_ERR, "socket(2) error when creating main server socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(LISTEN_PORT);
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
		syslog(LOG_ERR, "bind(2) error on main socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(sockfd, BACKLOG_SZ) < 0) {
		syslog(LOG_ERR, "listen(2) error on main socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	int clientfd;

	while ((clientfd = accept(sockfd, NULL, NULL)) >= 0) {

		if (accept_client(clientfd) < 0) {
			syslog(LOG_ERR, "Error accepting client: %s\n", strerror(errno));
		}

	}

	if (clientfd < 0) {
		syslog(LOG_ERR, "accept(2) error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

int accept_client(int clientfd) {

	pid_t pid;
	if ((pid = fork()) < 0) {
		return -1;
	}

	if (pid != 0) {
		close(clientfd);
		waitpid(pid, NULL, 0);
		return 0;
	}

	/* We fork again and terminate the first child to force the new child to become orphan,
	 * so we don't have to worry about reaping its termination status. This makes the first
	 * wait relatively quick (it's just the time to fork again and exit), and simplifies the
	 * design because the server process can simply fork and forget about it, and we won't leave
	 * zombie processes around.
	 */
	if ((pid = fork()) < 0) {
		syslog(LOG_ERR, "fork(2) failed on 1st child in accept_client(): %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (pid != 0)
		exit(EXIT_SUCCESS);

	if (dup2(clientfd, STDIN_FILENO) < 0) {
		syslog(LOG_ERR, "dup2(2) failed when attempting to duplicate onto stdin: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (dup2(clientfd, STDOUT_FILENO) < 0) {
		syslog(LOG_ERR, "dup2(2) failed when attempting to duplicate onto stdout: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (clientfd != STDIN_FILENO && clientfd != STDOUT_FILENO) {
		if (close(clientfd) < 0) {
			syslog(LOG_ERR,
			       "Warning: close(2) error after successfully duplicating it to stdin and stdout: %s\n",
			       strerror(errno));
		}
	}

	if (execl(PTY_PATH, PTY_PATH, LOGIN_PATH, NULL) < 0) {
		syslog(LOG_ERR, "execl(2) error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Never reached */
	assert(0);
}
