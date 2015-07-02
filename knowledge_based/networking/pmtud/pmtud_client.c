
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s IP { Y | N } (Y = Perform PMTUD, N = Don't perform PMTUD)\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (argv[2][0] != 'Y' && argv[2][0] != 'N') {
		fprintf(stderr, "Please enter Y or N\n");
		exit(EXIT_FAILURE);
	}

	static char sendbuff[5000];
	size_t i;
	for (i = 0; i < sizeof(sendbuff); i++) {
		sendbuff[i] = 'a'+i%26;
	}

	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(12000);
	int s = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	if (s <= 0) {
		if (s == 0) {
			fprintf(stderr, "Invalid server IP: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		} else {
			perror("inet_pton(3) error");
			exit(EXIT_FAILURE);
		}
	}

	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket(2) error");
		exit(EXIT_FAILURE);
	}

	int ip_mtu_disc_opt = 1;

	if (argv[1][0] == 'N') {
		ip_mtu_disc_opt = 0;
	}

	if (setsockopt(sockfd, IPPROTO_IP, IP_MTU_DISCOVER, &ip_mtu_disc_opt, sizeof(ip_mtu_disc_opt)) < 0) {
		perror("setsockopt(2) error");
		exit(EXIT_FAILURE);
	}

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		perror("Connection error");
		exit(EXIT_FAILURE);
	}

	while (1) {

		/* Let's check the current known path MTU */
		int pathmtu;
		socklen_t pathmtusz = sizeof(pathmtu);
		if (getsockopt(sockfd, IPPROTO_IP, IP_MTU, &pathmtu, &pathmtusz) < 0) {
			perror("getsockopt(2) error");
		} else {
			printf("Top of main loop; Path MTU = %d\n", pathmtu);
		}

		ssize_t sent = send(sockfd, sendbuff, sizeof(sendbuff), 0);
		if (sent < 0) {
			perror("send(2) error");
			exit(EXIT_FAILURE);
		}

		printf("*** Sent %zd bytes\n", sent);
		printf("Press Q to quit; return to continue\n");
		if (getchar() == 'Q') {
			break;
		}
	}

	return 0;
}
