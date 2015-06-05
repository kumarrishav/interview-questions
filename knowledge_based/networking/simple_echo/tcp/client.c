
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char buf[512];

int main(void) {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror("Couldn't create socket");
		exit(EXIT_FAILURE);
	}

	printf("Server's IP? ");
	scanf("%s", buf);

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(12000);
	if (inet_pton(AF_INET, buf, &serv_addr.sin_addr) <= 0) {
		perror("Invalid server address");
		exit(EXIT_FAILURE);
	}

	if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Couldn't connect to server");
		exit(EXIT_FAILURE);
	}

	printf("*** Connected to server\n");
	fgets(buf, sizeof(buf), stdin);
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		size_t to_send = strlen(buf);
		if (send(sock, buf, to_send, 0) != to_send) {
			perror("Unable to send data");
		} else {
			printf("*** Sent: %s", buf);
		}

		printf("*** Fetching data from server\n");

		ssize_t received = recv(sock, buf, sizeof(buf)-1, 0);
		if (received < 0) {
			perror("Couldn't fetch data");
		} else {
			buf[received] = '\0';
			printf("*** Received: %s", buf);
		}
	}

	static char quit_msg[] = "***QUIT***";
	send(sock, quit_msg, sizeof(quit_msg)-1, 0);
	close(sock);

	return 0;
}
