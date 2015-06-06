
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#define PING_PORT 12000
#define PING_PORT_STR "12000"
#define PING_MESSAGES 10
#define TIMEOUT_SECS 1

void do_ping(const char *target, int sock, struct sockaddr *dest, socklen_t dest_sz) {
	static const char ping_request[] = "PING\r\n";
	static const char ping_reply[] = "PONG\r\n";
	static char sock_buff[1024];

	int i;
	for (i = 0; i < PING_MESSAGES; i++) {
		printf("PING %s: ", target);

		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(sock, &fdset);

		struct timeval timeout;
		timeout.tv_sec = TIMEOUT_SECS;
		timeout.tv_usec = 0;

		struct timeval before;
		gettimeofday(&before, NULL);

		if (sendto(sock, ping_request, sizeof(ping_request)-1, 0, dest, dest_sz) < 0) {
			printf("error - %s\n", strerror(errno));
			continue;
		}

		int res = select(sock+1, &fdset, NULL, NULL, &timeout);
		struct timeval after;
		gettimeofday(&after, NULL);

		if (res < 0) {
			printf("error - %s\n", strerror(errno));
		} else if (res == 0) {
			printf("TIMEOUT\n");
		} else {
			ssize_t received;
			if ((received = recvfrom(sock, sock_buff, sizeof(sock_buff)-1, 0, NULL, NULL)) < 0) {
				printf("error - %s\n", strerror(errno));
				continue;
			}

			sock_buff[received] = '\0';
			if (strcmp(sock_buff, ping_reply)) {
				printf("error - unexpected reply\n");
			} else {
				double millisecs = (after.tv_sec-before.tv_sec)*1000+(after.tv_usec-before.tv_usec)/1000.0;
				printf("%.4f ms\n", millisecs);
			}
		}
	}
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s hostname/ip\n", argv[0]);
		return 0;
	}

	struct addrinfo *res;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	int s;
	if ((s = getaddrinfo(argv[1], PING_PORT_STR, &hints, &res)) != 0) {
		fprintf(stderr, "Couldn't locate PING server: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	if (res == NULL) {
		fprintf(stderr, "Couldn't find PING service on the target host.\n");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}

	if (res->ai_next != NULL) {
		// Should never happen
		fprintf(stderr, "Oops! Multiple PING services found on the targest host?!\n");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		perror("Unable to create socket");
		freeaddrinfo(res);
		exit(EXIT_FAILURE);
	}

	do_ping(argv[1], sock, res->ai_addr, res->ai_addrlen);

	freeaddrinfo(res);

	return 0;
}
