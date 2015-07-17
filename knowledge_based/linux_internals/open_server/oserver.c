#define _GNU_SOURCE
#include "oserver.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int send_fd(int sockfd, int fd_to_send) {
	int status = 0;

	struct iovec iov;
	iov.iov_base = &status;
	iov.iov_len = sizeof(status);

	unsigned char cmsg_buff[CMSG_SPACE(sizeof(fd_to_send))+CMSG_SPACE(sizeof(struct ucred))];

	/* Needed because of a bug in glibc
	 * (see https://sourceware.org/bugzilla/show_bug.cgi?id=13500)
	 *
	 * In particular, CMSG_NXTHDR assumes that cmsg_len has been initialized and it attempts
	 * to read it for some additional validation. This may cause it to return NULL when it
	 * shouldn't, which in turn caused segfaults in this function when it tried to
	 * append the credentials.
	 *
	 */
	memset(cmsg_buff, 0, sizeof(cmsg_buff));

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = sizeof(cmsg_buff);
	msg.msg_flags = 0;

	struct cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(fd_to_send));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	* (int *) CMSG_DATA(cmptr) = fd_to_send;

	cmptr = CMSG_NXTHDR(&msg, cmptr);
	cmptr->cmsg_len = CMSG_LEN(sizeof(struct ucred));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_CREDENTIALS;

	struct ucred *cred_ptr = (struct ucred *) CMSG_DATA(cmptr);
	cred_ptr->pid = getpid();
	cred_ptr->uid = geteuid();
	cred_ptr->gid = getegid();

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

	struct iovec iov;
	iov.iov_base = &errno;
	iov.iov_len = sizeof(errno);

	unsigned char cmsg_buff[CMSG_SPACE(sizeof(struct ucred))];
	/* Just in case...
	 * See the comment in send_fd()
	 */
	memset(cmsg_buff, 0, sizeof(cmsg_buff));

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = sizeof(cmsg_buff);
	msg.msg_flags = 0;

	struct cmsghdr *cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(struct ucred));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_CREDENTIALS;

	struct ucred *cred_ptr = (struct ucred *) CMSG_DATA(cmptr);
	cred_ptr->pid = getpid();
	cred_ptr->uid = geteuid();
	cred_ptr->gid = getegid();

	ssize_t sent = sendmsg(sockfd, &msg, 0);

	if (sent < 0)
		return -1;

	if (sent != sizeof(errno)) {
		errno = ECOMM;
		return -1;
	}

	return 0;
}

int recv_fd(int sockfd, struct ucred *server) {
	int status;
	unsigned char cmsg_buff[CMSG_SPACE(sizeof(status))+CMSG_SPACE(sizeof(*server))];
	memset(cmsg_buff, 0, sizeof(cmsg_buff));

	struct iovec iov;
	iov.iov_base = &status;
	iov.iov_len = sizeof(status);

	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg_buff;
	msg.msg_controllen = sizeof(cmsg_buff);
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
		if (msg.msg_controllen != CMSG_SPACE(sizeof(*server))) {
			errno = EBADMSG;
		} else {
			errno = status;
			if (server != NULL) {
				*server = * (struct ucred *) CMSG_DATA(CMSG_FIRSTHDR(&msg));
			}
		}
		return -1;
	}

	if (msg.msg_controllen != sizeof(cmsg_buff)) {
		errno = EBADMSG;
		return -1;
	}

	struct ucred *creds_ptr = NULL;
	int *fd_ptr = NULL;
	struct cmsghdr *cmptr;

	for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL; cmptr = CMSG_NXTHDR(&msg, cmptr)) {
		if (cmptr->cmsg_type == SCM_CREDENTIALS) {
			creds_ptr = (struct ucred *) CMSG_DATA(cmptr);
		} else if (cmptr->cmsg_type == SCM_RIGHTS) {
			fd_ptr = (int *) CMSG_DATA(cmptr);
		}
	}

	if (fd_ptr == NULL || creds_ptr == NULL) {
		errno = EBADMSG;
		return -1;
	}

	if (server != NULL)
		*server = *creds_ptr;

	return *fd_ptr;
}
