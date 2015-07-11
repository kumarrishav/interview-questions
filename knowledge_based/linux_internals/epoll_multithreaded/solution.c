/* This code implements an echo server using a thread pool that multiplexes requests and new
 * connections using the epoll(7) interface.
 *
 * This surfaced after a nice question about the multithreading capabilities of epoll on
 * StackOverflow. See http://stackoverflow.com/questions/31357215
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SERVERPORT 3000
#define SERVERBACKLOG 10
#define THREADSNO 20
#define EVENTS_BUFF_SZ 256

static int serversock;
static int epoll_fd;
static pthread_t threads[THREADSNO];

int accept_new_client(void) {

	int clientsock;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	if ((clientsock = accept(serversock, (struct sockaddr *) &addr, &addrlen)) < 0) {
		return -1;
	}

	char ip_buff[INET_ADDRSTRLEN+1];
	if (inet_ntop(AF_INET, &addr.sin_addr, ip_buff, sizeof(ip_buff)) == NULL) {
		close(clientsock);
		return -1;
	}

	printf("*** [%p] Client connected from %s:%" PRIu16 "\n", (void *) pthread_self(),
	       ip_buff, ntohs(addr.sin_port));

	struct epoll_event epevent;
	epevent.events = EPOLLIN | EPOLLET;
	epevent.data.fd = clientsock;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientsock, &epevent) < 0) {
		perror("epoll_ctl(2) failed attempting to add new client");
		close(clientsock);
		return -1;
	}

	return 0;
}

int handle_request(int clientfd) {
	char readbuff[512];
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t n;

	if ((n = recv(clientfd, readbuff, sizeof(readbuff)-1, 0)) < 0) {
		return -1;
	}

	if (n == 0) {
		return 0;
	}

	readbuff[n] = '\0';

	if (getpeername(clientfd, (struct sockaddr *) &addr, &addrlen) < 0) {
		return -1;
	}

	char ip_buff[INET_ADDRSTRLEN+1];
	if (inet_ntop(AF_INET, &addr.sin_addr, ip_buff, sizeof(ip_buff)) == NULL) {
		return -1;
	}

	printf("*** [%p] [%s:%" PRIu16 "] -> server: %s", (void *) pthread_self(),
	       ip_buff, ntohs(addr.sin_port), readbuff);

	ssize_t sent;
	if ((sent = send(clientfd, readbuff, n, 0)) < 0) {
		return -1;
	}

	readbuff[sent] = '\0';

	printf("*** [%p] server -> [%s:%" PRIu16 "]: %s", (void *) pthread_self(),
	       ip_buff, ntohs(addr.sin_port), readbuff);

	return 0;
}

void *worker_thr(void *args) {
	struct epoll_event *events = malloc(sizeof(*events)*EVENTS_BUFF_SZ);
	if (events == NULL) {
		perror("malloc(3) failed when attempting to allocate events buffer");
		pthread_exit(NULL);
	}

	int events_cnt;
	while ((events_cnt = epoll_wait(epoll_fd, events, EVENTS_BUFF_SZ, -1)) > 0) {
		int i;
		for (i = 0; i < events_cnt; i++) {
			assert(events[i].events & EPOLLIN);

			if (events[i].data.fd == serversock) {
				if (accept_new_client() == -1) {
					fprintf(stderr, "Error accepting new client: %s\n",
						strerror(errno));
				}
			} else {
				if (handle_request(events[i].data.fd) == -1) {
					fprintf(stderr, "Error handling request: %s\n",
						strerror(errno));
				}
			}
		}
	}

	if (events_cnt == 0) {
		fprintf(stderr, "epoll_wait(2) returned 0, but timeout was not specified...?");
	} else {
		perror("epoll_wait(2) error");
	}

	free(events);

	return NULL;
}

int main(void) {
	if ((serversock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(2) failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(serversock, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
		perror("bind(2) failed");
		exit(EXIT_FAILURE);
	}

	if (listen(serversock, SERVERBACKLOG) < 0) {
		perror("listen(2) failed");
		exit(EXIT_FAILURE);
	}

	if ((epoll_fd = epoll_create(1)) < 0) {
		perror("epoll_create(2) failed");
		exit(EXIT_FAILURE);
	}

	struct epoll_event epevent;
	epevent.events = EPOLLIN | EPOLLET;
	epevent.data.fd = serversock;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serversock, &epevent) < 0) {
		perror("epoll_ctl(2) failed on main server socket");
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 0; i < THREADSNO; i++) {
		if (pthread_create(&threads[i], NULL, worker_thr, NULL) < 0) {
			perror("pthread_create(3) failed");
			exit(EXIT_FAILURE);
		}
	}

	/* main thread also contributes as worker thread */
	worker_thr(NULL);

	return 0;
}
