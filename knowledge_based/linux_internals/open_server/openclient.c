
#define _GNU_SOURCE
#include "oserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

/* Returns 0 if the server referred to by `creds` is not trusted, otherwise returns 1 */
int auth_server(const struct ucred *creds) {
	/* This is just a stub. We could now create custom authentication rules and authenticate the
	 * server accordingly
	 */
	return 1;
}

int serv_open(char *path, int mode) {
	static char buff[4096];
	int req_len;

	if ((req_len = snprintf(buff, sizeof(buff), "open %s %d\n", path, mode)) >= sizeof(buff)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	int sockfds[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds) < 0) {
		return -1;
	}

	int optval = 1;
	if (setsockopt(sockfds[1], SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0) {
		close(sockfds[0]);
		close(sockfds[1]);
		return -1;
	}

	/* Block SIGCHLD */
	sigset_t old, new;
	sigemptyset(&new);
	sigaddset(&new, SIGCHLD);
	sigprocmask(SIG_BLOCK, &new, &old);

	pid_t pid;
	if ((pid = fork()) < 0) {
		close(sockfds[0]);
		close(sockfds[1]);
		sigprocmask(SIG_SETMASK, &old, NULL);
		return -1;
	}

	int ret;

	if (pid == 0) {
		/* Child executes the open server */
		close(sockfds[1]);
		if (dup2(sockfds[0], STDIN_FILENO) < 0) {
			perror("dup2(2) for stdin failed");
			exit(EXIT_FAILURE);
		}
		if (dup2(sockfds[0], STDOUT_FILENO) < 0) {
			perror("dup2(2) for stdout failed");
			exit(EXIT_FAILURE);
		}

		if (sockfds[0] != STDIN_FILENO && sockfds[0] != STDOUT_FILENO) {
			close(sockfds[0]);
		}
		if (execl("./openserver", "./openserver", NULL) < 0) {
			perror("execl(2) failed");
			exit(EXIT_FAILURE);
		}
	}

	close(sockfds[0]);

	struct ucred creds;
	creds.pid = getpid();
	creds.uid = geteuid();
	creds.gid = getegid();

	unsigned char cmsg_buff[CMSG_SPACE(sizeof(creds))];
	struct cmsghdr *cmptr = (struct cmsghdr *) cmsg_buff;
	cmptr->cmsg_len = CMSG_LEN(sizeof(creds));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_CREDENTIALS;
	* (struct ucred *) CMSG_DATA(cmptr) = creds;

	struct iovec iov;
	iov.iov_base = buff;
	iov.iov_len = req_len;

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = cmptr->cmsg_len;
	msg.msg_flags = 0;

	ssize_t sent;
	if ((sent = sendmsg(sockfds[1], &msg, 0)) != req_len) {
		ret = -1;
		if (sent > 0)
			errno = ECOMM;
		goto out;
	}

	struct ucred server_creds;
	ret = recv_fd(sockfds[1], &server_creds);
	if ((ret < 0 && errno == EBADMSG) || !auth_server(&server_creds)) {
		/* Server didn't send credentials or it did but we don't trust */
		errno = EBADE;
		ret = -1;
		goto out;
	}

out:
	close(sockfds[1]);
	waitpid(pid, NULL, 0);
	sigprocmask(SIG_SETMASK, &old, NULL);
	return ret;
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename1 [filename2] [filename3] ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 1; i < argc; i++) {
		int fd = serv_open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "Couldn't open %s: %s\n", argv[i], strerror(errno));
			continue;
		}

		FILE *fptr = fdopen(fd, "r");
		if (fptr == NULL) {
			fprintf(stderr, "Couldn't get file pointer for %s: %s\n",
				argv[i], strerror(errno));
			close(fd);
			continue;
		}

		int c;
		while ((c = getc(fptr)) != EOF)
			putchar(c);
		fclose(fptr);
	}

	return 0;
}
