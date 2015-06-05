
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

static char buf[512];

int main(void) {
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in dest;
	dest.sin_family = AF_INET;
	dest.sin_port = htons(12000);

	printf("Server's IP? ");
	scanf("%s", buf);

	if (inet_pton(AF_INET, buf, &dest.sin_addr) <= 0) {
		perror("Invalid server address");
		exit(EXIT_FAILURE);
	}
	fgets(buf, sizeof(buf), stdin);
	printf("Enter a line to send to the server\n");
	printf("> ");
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		size_t to_send = strlen(buf);
		if (sendto(sock, buf, to_send, 0, (struct sockaddr *) &dest, sizeof(dest)) != to_send) {
			perror("Error sending data");
		} else {
			printf("*** Sent: %s", buf);
		}

		printf("*** Fetching data from server...\n");
		ssize_t received = recv(sock, buf, sizeof(buf)-1, 0);
		if (received == 0) {
			printf("Shutting down\n");
			break;
		} else if (received < 0) {
			perror("Error receiving data");
		} else {
			buf[received] = '\0';
			printf("*** Received: %s", buf);
		}

		printf("> ");
	}

	return 0;
}
