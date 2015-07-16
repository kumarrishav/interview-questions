/* An openserver exec()'d by the client */

#define _GNU_SOURCE
#include "oserver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#define LINE_MAX 4096

/* Returns 0 if the process identified by the credentials in `who` *CANNOT* access the file
 * in `path`; otherwise returns non-zero
 */
int can_access(const char *path, const struct ucred *who) {
	/* This is just a stub. We could now create custom permission rules and enforce them
	 * using this function.
	 */
	return 1;
}

void handle_request(char *request, const struct ucred *src) {
	char *buff;
	int omode;

	if ((buff = strdup(request)) == NULL) {
		perror("strdup(3) couldn't allocate memory");
		return;
	}

	if (sscanf(request, "open %s %d", buff, &omode) != 2) {
		fprintf(stderr, "Invalid request: %s\n", request);
		goto outbuff;
	}

	if (!can_access(buff, src)) {
		errno = EACCES;
		if (send_errno(STDOUT_FILENO) < 0) {
			fprintf(stderr, "send_errno() failed attempting to notify client: %s\n",
				strerror(errno));
			fprintf(stderr, "Note: Permission denied for pid %ld to open %s (euid = %ld, egid = %ld)\n",
				(long) src->pid, buff, (long) src->uid, (long) src->gid);
		}
		goto outbuff;
	}

	int fd = open(buff, omode);

	if (fd < 0) {
		int open_error = errno;
		if (send_errno(STDOUT_FILENO) < 0) {
			fprintf(stderr, "send_errno() failed attempting to notify client: %s\n",
				strerror(errno));
			fprintf(stderr, "Note: couldn't open %s: %s\n", buff, strerror(open_error));
		}
		goto outbuff;
	}
	if (send_fd(STDOUT_FILENO, fd) < 0) {
		fprintf(stderr, "send_fd() failed attempting to send fd for %s: %s\n",
			buff, strerror(errno));
		goto outclose;
	}

outclose:
	close(fd);
outbuff:
	free(buff);
}

int main(void) {

	struct stat statbuf;
	if (fstat(STDIN_FILENO, &statbuf) < 0) {
		perror("fstat(2) failed for stdin");
		exit(EXIT_FAILURE);
	}
	if (!S_ISSOCK(statbuf.st_mode)) {
		fprintf(stderr, "Fatal: stdin is not a socket\n");
		exit(EXIT_FAILURE);
	}
	if (fstat(STDOUT_FILENO, &statbuf) < 0) {
		perror("fstat(2) failed for stdout");
		exit(EXIT_FAILURE);
	}
	if (!S_ISSOCK(statbuf.st_mode)) {
		fprintf(stderr, "Fatal: stdout is not a socket\n");
		exit(EXIT_FAILURE);
	}

	int optval = 1;	
	if (setsockopt(STDOUT_FILENO, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) < 0) {
		perror("setsockopt(2) failed when trying to set SO_PASSCRED");
		exit(EXIT_FAILURE);
	}

	static char linebuff[LINE_MAX];
	ssize_t msg_len;

	unsigned char cmsg_buff[CMSG_SPACE(sizeof(struct ucred))];

	struct iovec iov;
	iov.iov_base = linebuff;
	iov.iov_len = sizeof(linebuff)-1;

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = CMSG_LEN(sizeof(struct ucred));
	msg.msg_flags = 0;

	while ((msg_len = recvmsg(STDIN_FILENO, &msg, 0)) > 0) {
		char *request = iov.iov_base;
		if (request[msg_len-1] == '\n')
			request[--msg_len] = '\0';
		if (msg.msg_controllen != CMSG_LEN(sizeof(struct ucred))) {
			errno = EINVAL;
			if (send_errno(STDOUT_FILENO) < 0) {
				fprintf(stderr, "send_errno() failed when attempting to notify client of invalid or non-existent cmsg: %s\n",
					strerror(errno));
			}
		} else {
			struct ucred *ucredptr;
			ucredptr = (struct ucred *) CMSG_DATA((struct cmsghdr *) cmsg_buff);
			handle_request(request, ucredptr);
		}

		iov.iov_base = linebuff;
		iov.iov_len = sizeof(linebuff)-1;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_control = cmsg_buff;
		msg.msg_controllen = CMSG_LEN(sizeof(struct ucred));
		msg.msg_flags = 0;
	}

	if (msg_len < 0) {
		perror("recvmsg(2) error");
		exit(EXIT_FAILURE);
	}

	return 0;
}
