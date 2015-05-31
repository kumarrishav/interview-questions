#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static struct server server;

void shutdown_handler(int signo) {
	destroy_server(&server);
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

	setup_signals();

	printf("*** Creating a server...\n");

	if (new_server("server1.example.com", &server) < 0) {
		perror("Couldn't create server");
		exit(EXIT_FAILURE);
	}

	printf("*** Created!\n"
	       "    This is an echo server.\n"
	       "    Anything received will be echoed back to the client.\n\n");

	/* Accept a client */
	struct comm_channel ch;
	if (accept_client(&server, &ch) < 0) {
		perror("Couldn't accept client");
		destroy_server(&server);
		exit(EXIT_FAILURE);
	}

	printf("*** Accepted incoming connection request.\n");

	static char buf[512+1];
	int n;

	while ((n = recv_info(&ch, buf, sizeof(buf)-1)) > 0) {
		buf[n] = '\0';
		if (!strcmp(buf, "__QUIT__NOW__\n")) {
			printf("*** Got disconnect request from client, leaving...\n");
			disconnect_from(&ch);
			break;
		}

		printf("*** Received: %s", buf);

		if (send_info(&ch, buf, n) < 0) {
			perror("Error sending message to client");
		} else {
			printf("*** Sent: %s", buf);
		}
	}

	printf("*** Goodbye\n");

	destroy_server(&server);

	return 0;
}
