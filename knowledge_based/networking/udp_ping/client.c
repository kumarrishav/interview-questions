/* Implement a simple PING service on top of UDP.
 *
 * Your ping program is to send 10 ping messages to the target server over UDP.
 * For each message, your client is to determine and print the RTT (Round Trip Time)
 * when the corresponding message is returned.
 *
 * Because UDP is an unreliable protocol, a packet sent by the client or server may be lost.
 * For this reason, the client cannot wait indefinitely for a reply to a ping message.
 * You should have the client wait up to one second for a reply from the server; if no reply is
 * received, the client should assume that the packet was lost and print a message accordingly.
 *
 * Source: Adapted from Kurose & Ross
 */
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

/* Some interesting times measured when testing this code:
 *
 * Source machine running the client was located in Lisbon, Portugal.
 *
 * It is interesting to see how ping times increase with distance.
 *
 * Results for a virtual server from DigitalOcean running in San Francisco, CA, where
 * packets (probably) had to go through a transatlantic link (either that, or satellite).
 *
 * PING 104.***.***.***: 209.3490 ms
 * PING 104.***.***.***: 204.2170 ms
 * PING 104.***.***.***: 204.0840 ms
 * PING 104.***.***.***: 204.0750 ms
 * PING 104.***.***.***: 204.1260 ms
 * PING 104.***.***.***: 203.5260 ms
 * PING 104.***.***.***: 203.9290 ms
 * PING 104.***.***.***: 203.9460 ms
 * PING 104.***.***.***: 204.1560 ms
 * PING 104.***.***.***: 203.7490 ms
 *
 * Results for an OVH server running in Roubaix, France.
 * It is expected that the ping times are slightly lower, since the packets never leave Europe.
 * Also, this is a dedicated server rather than a VPS, but this is usually irrelevant.
 *
 * PING *.kimsufi.com: 44.2090 ms
 * PING *.kimsufi.com: 40.5510 ms
 * PING *.kimsufi.com: 39.9340 ms
 * PING *.kimsufi.com: 39.9480 ms
 * PING *.kimsufi.com: 40.1100 ms
 * PING *.kimsufi.com: 40.8410 ms
 * PING *.kimsufi.com: 40.0790 ms
 * PING *.kimsufi.com: 41.0890 ms
 * PING *.kimsufi.com: 40.8160 ms
 * PING *.kimsufi.com: 39.8980 ms
 *
 * Results for a server running in the same LAN as the client:
 * PING 192.168.1.91: 1.1860 ms
 * PING 192.168.1.91: 0.5220 ms
 * PING 192.168.1.91: 0.7420 ms
 * PING 192.168.1.91: 0.6920 ms
 * PING 192.168.1.91: 0.4170 ms
 * PING 192.168.1.91: 0.7390 ms
 * PING 192.168.1.91: 0.4060 ms
 * PING 192.168.1.91: 0.3660 ms
 * PING 192.168.1.91: 0.3530 ms
 * PING 192.168.1.91: 0.3630 ms
 *
 * Results for localhost:
 * PING localhost: 0.1810 ms
 * PING localhost: 0.1460 ms
 * PING localhost: 0.1470 ms
 * PING localhost: 0.1470 ms
 * PING localhost: 0.1470 ms
 * PING localhost: 0.1460 ms
 * PING localhost: 0.1180 ms
 * PING localhost: 0.0410 ms
 * PING localhost: 0.0840 ms
 * PING localhost: 0.0730 ms
 *
 */

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
