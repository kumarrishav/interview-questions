#include "api.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

int new_server(const char *name) {
	int sockfd;
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	size_t name_len = strlen(name)+sizeof(CLIENT_SOCK_DIR)+1;
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	if (name_len > sizeof(addr.sun_path)) {
		errno = ENAMETOOLONG;
		goto errout;
	}

	static char buff[sizeof(addr.sun_path)+1];
	sprintf(buff, "%s/%s", CLIENT_SOCK_DIR, name);

	unlink(buff); /* In case it already exists */

	memcpy(&addr.sun_path, buff, name_len);
	size_t addr_sz = sizeof(addr)-sizeof(addr.sun_path)+name_len;

	if (bind(sockfd, (struct sockaddr *) &addr, addr_sz) < 0) {
		goto errout;
	}

	if (listen(sockfd, BACKLOG_SZ) < 0) {
		goto errout;
	}

	return sockfd;

errout:
	close(sockfd);
	return -1;
}

void destroy_server(int servfd) {
	close(servfd);
}

int accept_client(int servfd) {
	struct sockaddr_un client_addr;
	socklen_t client_addr_sz = sizeof(client_addr);
	static char buff[sizeof(client_addr.sun_path)+1];

	int clientfd;
	if ((clientfd = accept(servfd, (struct sockaddr *) &client_addr, &client_addr_sz)) < 0) {
		return -1;
	}

	sprintf(buff, "%s", client_addr.sun_path);

	struct stat statbuff;
	if (stat(buff, &statbuff) < 0)
		goto errout;

	unlink(buff); /* This is no longer needed */

	if (!S_ISSOCK(statbuff.st_mode)) {
		errno = EINVAL;
		goto errout;
	}

	if ((statbuff.st_mode & (S_IRWXG | S_IRWXO)) ||
	    ((statbuff.st_mode & CLIPERM) != CLIPERM)) {
		errno = EPERM;
		goto errout;
	}

	time_t lim = time(NULL) - STALE_TIME;
	if (statbuff.st_atime < lim || statbuff.st_mtime < lim || statbuff.st_ctime < lim) {
		errno = EPERM;
		goto errout;
	}

	return clientfd;

errout:
	close(clientfd);
	return -1;
}

int connect_to(const char *name) {
	int written;
	int sockfd;
	size_t cliaddr_sz;
	size_t servaddr_sz;
	struct sockaddr_un cliaddr;
	struct sockaddr_un serveraddr;

	cliaddr.sun_family = AF_UNIX;
	written = snprintf(cliaddr.sun_path, sizeof(cliaddr.sun_path)-1,
			   "%s/%ld", CLIENT_SOCK_DIR, (long) getpid());

	if (written >= sizeof(cliaddr.sun_path)-1) {
		errno = ENAMETOOLONG;
		return -1;
	}

	cliaddr_sz = sizeof(cliaddr)-sizeof(cliaddr.sun_path)+written;

	serveraddr.sun_family = AF_UNIX;
	written = snprintf(serveraddr.sun_path, sizeof(serveraddr.sun_path),
			   "%s/%s", CLIENT_SOCK_DIR, name);
	if (written > sizeof(serveraddr.sun_path)) {
		errno = ENAMETOOLONG;
		return -1;
	}

	servaddr_sz = sizeof(serveraddr)-sizeof(serveraddr.sun_path)+written;

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	unlink(cliaddr.sun_path);

	if (bind(sockfd, (struct sockaddr *) &cliaddr, cliaddr_sz) < 0) {
		goto errout;
	}

	if (chmod(cliaddr.sun_path, CLIPERM) < 0) {
		unlink(cliaddr.sun_path);
		goto errout;
	}

	if (connect(sockfd, (struct sockaddr *) &serveraddr, servaddr_sz) < 0) {
		goto errout;
	}

	return sockfd;
	
errout:
	close(sockfd);
	return -1;
}

int recv_info(int fd, void *buff, size_t buff_len) {
	return recv(fd, buff, buff_len, 0);
}

int send_info(int fd, void *buff, size_t buff_len) {
	return send(fd, buff, buff_len, 0);
}

void disconnect_from(int fd) {
	close(fd);
}
