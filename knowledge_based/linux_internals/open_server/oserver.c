#include "oserver.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int send_fd(int sockfd, int fd_to_send) {
	int status = 0;
	unsigned char cmsg_buff[CMSG_SPACE(sizeof(fd_to_send))];
	struct cmsghdr *cmptr = (struct cmsghdr *) cmsg_buff;

	cmptr->cmsg_len = CMSG_LEN(sizeof(fd_to_send));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	* (int *) CMSG_DATA(cmptr) = fd_to_send;

	struct iovec iov;
	iov.iov_base = &status;
	iov.iov_len = sizeof(status);

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = cmptr->cmsg_len;
	msg.msg_flags = 0;

	ssize_t sent = sendmsg(sockfd, &msg, 0);
	if (sent < 0)
		return -1;
	if (sent != sizeof(status)) {
		errno = ECOMM;
		return -1;
	}

	return 0;
}

int send_errno(int sockfd) {
	ssize_t sent = send(sockfd, &errno, sizeof(errno), 0);
	if (sent < 0)
		return -1;
	if (sent != sizeof(errno)) {
		errno = ECOMM;
		return -1;
	}
	return 0;
}

int recv_fd(int sockfd) {
	int status;
	unsigned char cmsg_buff[CMSG_SPACE(sizeof(status))];
	struct cmsghdr *cmptr = (struct cmsghdr *) cmsg_buff;

	struct iovec iov;
	iov.iov_base = &status;
	iov.iov_len = sizeof(status);

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmptr;
	msg.msg_controllen = CMSG_LEN(sizeof(status));
	msg.msg_flags = 0;

	ssize_t received = recvmsg(sockfd, &msg, 0);
	if (received < 0) {
		return -1;
	}
	if (received < sizeof(status)) {
		errno = ECOMM;
		return -1;
	}
	if (status != 0) {
		errno = status;
		return -1;
	}

	if (msg.msg_controllen != CMSG_SPACE(sizeof(status))) {
		errno = EBADFD;
		return -1;
	}

	return * (int *) CMSG_DATA(cmptr);
}
