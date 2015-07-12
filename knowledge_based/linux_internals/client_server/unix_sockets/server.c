#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

static int serverfd;

void shutdown_handler(int signo) {
	destroy_server(serverfd);
	exit(0);
}

void setup_signals(void) {
	struct sigaction sighandlers;

	sighandlers.sa_handler = shutdown_handler;
	sigemptyset(&sighandlers.sa_mask);
	sighandlers.sa_flags = 0;

	sigaction(SIGINT, &sighandlers, NULL);
	sigaction(SIGTERM, &sighandlers, NULL);
}

/* A simple echo server */

int main(void) {
	int clientfd;

	setup_signals();
	printf("*** Creating a server...\n");

	if ((serverfd = new_server("server1.example.com")) < 0) {
		perror("Couldn't create server");
		exit(EXIT_FAILURE);
	}

	printf("*** Created!\n"
	       "    This is an echo server.\n"
	       "    Anything received will be echoed back to the client.\n\n");

	/* Accept a client */
	if ((clientfd = accept_client(serverfd)) < 0) {
		perror("Couldn't accept client");
		destroy_server(serverfd);
		exit(EXIT_FAILURE);
	}

	printf("*** Accepted incoming connection request.\n");

	static char buf[512+1];
	int n;

	while ((n = recv_info(clientfd, buf, sizeof(buf)-1)) > 0) {
		buf[n] = '\0';
		if (!strcmp(buf, "__QUIT__NOW__\n")) {
			printf("*** Got disconnect request from client, leaving...\n");
			disconnect_from(clientfd);
			break;
		}

		printf("*** Received: %s", buf);

		if (send_info(clientfd, buf, n) < 0) {
			perror("Error sending message to client");
		} else {
			printf("*** Sent: %s", buf);
		}
	}

	printf("*** Goodbye\n");

	destroy_server(serverfd);

	return 0;
}
