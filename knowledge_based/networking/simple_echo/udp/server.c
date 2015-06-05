
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

static char buf[512];

int main(void) {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Couldn't bind socket address");
		exit(EXIT_FAILURE);
	}

	printf("Server initialized; waiting for data to arrive.\n");

	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);

	ssize_t received = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *) &client, &client_len);
	errno = 0;
	while (received > 0) {
		buf[received] = '\0';
		printf("*** Received: %s", buf);
		size_t i;
		for (i = 0; buf[i] != '\0'; i++) {
			buf[i] = toupper((unsigned char) buf[i]);
		}
		if (sendto(sock, buf, received, 0, (struct sockaddr *) &client, client_len) != received) {
			perror("Couldn't send back the data");
		} else {
			printf("*** Sent: %s", buf);
		}

		received = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr *) &client, &client_len);
	}

	if (errno != 0) {
		perror(NULL);
	}

	return 0;
}
