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
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PING_PORT 12000

int main(void) {
	static char recv_buff[1024];
	static const char ping_reply[] = "PONG\r\n";
	static char ip_buff[INET_ADDRSTRLEN+1];

	printf("*** Initializing ping server...\n");

	int ping_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ping_sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PING_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(ping_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Couldn't bind address to socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in client;
	socklen_t client_sz = sizeof(client);

	printf("Initialized; waiting for PING requests\n");

	while (1) {
		memset(&client, 0, sizeof(client));
		ssize_t received = recvfrom(ping_sock, recv_buff, sizeof(recv_buff)-1, 0, (struct sockaddr *) &client, &client_sz);

		assert(client_sz == sizeof(client));

		if (received < 0) {
			perror("Error receiving data from socket");
			continue;
		}

		recv_buff[received] = '\0';

		if (strcmp(recv_buff, "PING\r\n")) {
			fprintf(stderr, "Got unexpected message %s, ignoring...\n", recv_buff);
			continue;
		}

		if (inet_ntop(AF_INET, &client.sin_addr, ip_buff, client_sz) == NULL) {
			fprintf(stderr, "Couldn't determine client address");
			continue;
		}

		printf("*** [%s] -> PING\n", ip_buff);

		if (sendto(ping_sock, ping_reply, sizeof(ping_reply)-1, 0, (struct sockaddr *) &client, client_sz) < 0) {
			perror("Error sending reply to client");
		} else {
			printf("*** [%s] <- PONG\n", ip_buff);
		}
	}

	return 0;
}
