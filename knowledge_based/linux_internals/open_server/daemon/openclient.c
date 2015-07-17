
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
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_ADDRESS "/tmp/opend.socket"

/* Returns 0 if the server referred to by `creds` is not trusted, otherwise returns 1 */
int auth_server(const struct ucred *creds) {
	/* This is just a stub. We could now create custom authentication rules and authenticate the
	 * server accordingly
	 */
	return 1;
}

int serv_open(int servsock, char *path, int mode) {
	static char buff[4096];
	static char cwd_buff[4096];
	int req_len;
	int ret;

	if (path == NULL || *path == '\0') {
		errno = EINVAL;
		return -1;
	}

	cwd_buff[0] = '\0';

	if (*path != '/') {
		/* Convert to absolute pathname to send to daemon */
		if (getcwd(cwd_buff, sizeof(cwd_buff)-1) == NULL) {
			return -1;
		}
		size_t cwd_len = strlen(cwd_buff);
		if (cwd_len == 0 || cwd_buff[cwd_len-1] != '/') {
			cwd_buff[cwd_len] = '/';
			cwd_buff[cwd_len+1] = '\0';
		}
	}

	req_len = snprintf(buff, sizeof(buff), "open %s%s %d\n", cwd_buff, path, mode);
	if (req_len >= sizeof(buff)) {
		errno = ENAMETOOLONG;
		return -1;
	}

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
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = sizeof(cmsg_buff);
	msg.msg_flags = 0;

	ssize_t sent;
	if ((sent = sendmsg(servsock, &msg, 0)) != req_len) {
		if (sent > 0)
			errno = ECOMM;
		return -1;
	}

	struct ucred server_creds;
	ret = recv_fd(servsock, &server_creds);

	if ((ret < 0 && errno == EBADMSG) || !auth_server(&server_creds)) {
		/* Server didn't send credentials or it did but we don't trust */
		errno = EBADE;
		return -1;
	}

	return ret;
}

int serv_connect(const char *addr) {
	struct sockaddr_un sockaddr;
	size_t addr_strlen = strlen(addr);
	socklen_t sockaddr_len;

	sockaddr.sun_family = AF_UNIX;

	if (addr_strlen+1 > sizeof(sockaddr.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	strcpy(sockaddr.sun_path, addr);
	sockaddr_len = sizeof(sockaddr)-sizeof(sockaddr.sun_path)+addr_strlen;

	int sockfd;
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if (connect(sockfd, (struct sockaddr *) &sockaddr, sockaddr_len) < 0)
		goto errout;

	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0)
		goto errout;

	return sockfd;

errout:
	close(sockfd);
	return -1;
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename1 [filename2] [filename3] ...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int sockfd;
	if ((sockfd = serv_connect(SERVER_ADDRESS)) < 0) {
		perror("Couldn't connect to opend socket");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 1; i < argc; i++) {
		int fd = serv_open(sockfd, argv[i], O_RDONLY);
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
