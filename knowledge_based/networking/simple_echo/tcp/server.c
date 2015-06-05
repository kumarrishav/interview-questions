
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#define BACKLOG 16

static char buf[512];

int main(void) {
	int main_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (main_sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(12000);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(main_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Couldn't bind address");
		exit(EXIT_FAILURE);
	}

	if (listen(main_sock, BACKLOG) < 0) {
		perror("Couldn't listen");
		exit(EXIT_FAILURE);
	}

	int client = accept(main_sock, NULL, NULL);
	if (client < 0) {
		perror("Couldn't accept new client");
		exit(EXIT_FAILURE);
	}

	printf("Server initialized; waiting for data to arrive.\n");

	ssize_t received;
	while ((received = recv(client, buf, sizeof(buf)-1, 0)) > 0) {
		buf[received] = '\0';

		if (!strcmp(buf, "***QUIT***")) {
			break;
		}

		printf("*** Received: %s", buf);
		size_t i;
		for (i = 0; buf[i] != '\0'; i++) {
			buf[i] = toupper((unsigned char) buf[i]);
		}
		if (send(client, buf, received, 0) != received) {
			perror("Error sending data");
		} else {
			printf("*** Sent: %s", buf);
		}
	}

	if (errno != 0) {
		perror(NULL);
	}

	close(client);
	close(main_sock);

	return 0;
}
