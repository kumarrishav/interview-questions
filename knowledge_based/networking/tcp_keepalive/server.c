/* This code illustrates the usage of epoll(7) combined with TCP Keepalives.
 *
 * It serves multiple clients at the same time without the hassle and overhead of multithreading.
 * Instead, it uses epoll(7) to multiplex I/O efficiently. epoll(7) has multiple advantages over
 * select(2) / poll(2): epoll_wait(2) is O(1), the interface is cleaner to use, and user code is not
 * forced to iterate through every file descriptor being watched: instead, epoll_wait(2) returns an
 * array filled with the descriptors that need attention, and nothing else.
 *
 * Furthermore, descriptors are added and deleted using epoll_ctl(2), so the kernel maintains the
 * information; user code is not required to do it, as opposed to select(2) and poll(2).
 *
 * The exact semantics of how to enable and configure TCP Keepalive are platform dependent. Thus,
 * this code is not POSIX compliant. Also, epoll(7) is Linux specific.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#define PORTNUMBER 3000
#define REMOVE_REQUESTED 0
#define CONN_DEAD 1

static const int client_keepcnt = 3;
static const int client_keepidle = 10;
static const int client_keepintvl = 5;

void accept_new_client(int epoll_fd, int server_sock);
void remove_client(int epoll_fd, int client_fd, int why);
void handle_client_req(int epoll_fd, int client_fd);

int main(void) {

	static struct epoll_event eventsbuff[1024];

	printf("*** Initializing server\n");

	int mainsock;
	if ((mainsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(2) failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORTNUMBER);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(mainsock, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		perror("bind(2) failed");
		exit(EXIT_FAILURE);
	}

	if (listen(mainsock, 10) < 0) {
		perror("listen(2) failed");
		exit(EXIT_FAILURE);
	}

	int epoll_fd;
	if ((epoll_fd = epoll_create(10)) < 0) {
		perror("epoll_create(2) failed");
		exit(EXIT_FAILURE);
	}

	struct epoll_event epevent;
	epevent.events = EPOLLIN | EPOLLHUP;
	epevent.data.fd = mainsock;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, mainsock, &epevent) < 0) {
		perror("epoll_ctl(2) failed");
		exit(EXIT_FAILURE);
	}

	printf("*** Server initialized; entering main loop\n");

	while (1) {
		int n;
		if ((n = epoll_wait(epoll_fd, eventsbuff, sizeof(eventsbuff), -1)) < 0) {
			perror("epoll_wait(2) error");
			exit(EXIT_FAILURE);
		}

		int i;
		for (i = 0; i < n; i++) {
			if (eventsbuff[i].data.fd == mainsock) {
				accept_new_client(epoll_fd, mainsock);
			} else if (eventsbuff[i].events & EPOLLHUP) {
				remove_client(epoll_fd, eventsbuff[i].data.fd, CONN_DEAD);
			} else {
				assert(eventsbuff[i].events & EPOLLIN);
				handle_client_req(epoll_fd, eventsbuff[i].data.fd);
			}
		}
	}
}

void accept_new_client(int epoll_fd, int server_sock) {
	printf("*** Accepting new client\n");

	int client = accept(server_sock, NULL, NULL);
	if (client < 0) {
		perror("accept(2) failed");
		return;
	}

	if (setsockopt(client, IPPROTO_TCP, TCP_KEEPCNT, &client_keepcnt, sizeof(client_keepcnt)) < 0) {
		perror("setsockopt(2) failed for TCP_KEEPCNT");
		return;
	}

	if (setsockopt(client, IPPROTO_TCP, TCP_KEEPIDLE, &client_keepidle, sizeof(client_keepidle)) < 0) {
		perror("setsockopt(2) failed for TCP_KEEPIDLE");
		return;
	}

	if (setsockopt(client, IPPROTO_TCP, TCP_KEEPINTVL, &client_keepintvl, sizeof(client_keepintvl)) < 0) {
		perror("setsockopt(2) failed for TCP_KEEPINTVL");
		return;
	}

	struct epoll_event epevent;
	epevent.events = EPOLLIN;
	epevent.data.fd = client;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &epevent) < 0) {
		perror("epoll_ctl(2) failed for new client");
		return;
	}

	printf("*** Accepted\n");
}

void remove_client(int epoll_fd, int client_fd, int why) {
	if (why == CONN_DEAD) {
		printf("*** Removing client because connection appears to be dead\n");
	} else if (why == REMOVE_REQUESTED) {
		printf("*** Removing client after voluntary request\n");
	} else {
		printf("*** Removing client for unknown reasons\n");
	}

	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) < 0) {
		perror("epoll_ctl(2) failed when attempting to remove client\n");
		return;
	}

	close(client_fd);
}

void handle_client_req(int epoll_fd, int client_fd) {
	static char readbuff[1024];

	ssize_t n;
	if ((n = recv(client_fd, readbuff, sizeof(readbuff)-1, 0)) < 0) {
		perror("recv(2) failed");
		return;
	}

	readbuff[n] = '\0';

	if (n == 0) {
		remove_client(epoll_fd, client_fd, CONN_DEAD);
		return;
	}

	if (!strcmp(readbuff, "QUIT\r\n")) {
		remove_client(epoll_fd, client_fd, REMOVE_REQUESTED);
		return;
	}

	printf("*** Received: %s", readbuff);

	ssize_t to_send = n;
	if ((to_send = send(client_fd, readbuff, n, 0)) != n) {
		if (to_send < 0) {
			perror("write(2) failed");
		} else {
			fprintf(stderr, "Partial write detected (%zd/%zd)\n", to_send, n);
		}
		return;
	}

	printf("*** Echoed: %s", readbuff);
}
