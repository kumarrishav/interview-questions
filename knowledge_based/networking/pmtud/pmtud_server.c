
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void) {

	static char recv_buff[5000];

	int mainsock;
	if ((mainsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(2) failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(12000);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(mainsock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind(2) failed");
		exit(EXIT_FAILURE);
	}

	if (listen(mainsock, 10) < 0) {
		perror("listen(2) failed");
		exit(EXIT_FAILURE);
	}

	int sock;
	if ((sock = accept(mainsock, NULL, NULL)) < 0) {
		perror("accept(2) failed");
		exit(EXIT_FAILURE);
	}

	while (1) {

		ssize_t n;
		if ((n = read(sock, recv_buff, sizeof(recv_buff)-1)) < 0) {
			perror("read(2) failed");
			break;
		}

		recv_buff[n] = '\0';

		printf("Server received: %s\n", recv_buff);
	}

	return 0;
}
