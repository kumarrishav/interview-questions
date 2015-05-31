#include "api.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static struct comm_channel ch;

void shutdown_handler(int signo) {
	static char quitmsg[] = "__QUIT__NOW__\n";
	send_info(&ch, quitmsg, sizeof(quitmsg)-1);
	disconnect_from(&ch);
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

/* A client that interacts with the echo server */

int main(void) {

	printf("*** Connecting to server...\n");

	if (connect_to("server1.example.com", &ch) < 0) {
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}

	setup_signals();

	printf("*** Connected!\n");
	printf("    This is a client connected to an echo server.\n"
	       "    Any lines you enter will be echoed back by the server.\n"
	       "    To quit, send EOF (usually Ctrl+D).\n\n");

	static char buf[512];

	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		size_t line_len = strlen(buf);

		if (send_info(&ch, buf, line_len) < 0) {
			perror("Error sending data to server");
			continue;
		}

		printf("*** Sent: %s", buf);

		int n;
		if ((n = recv_info(&ch, buf, sizeof(buf)-1)) < 0) {
			perror("Error receiving data from server\n");
			continue;
		}

		buf[n] = '\0';
		printf("*** Received: %s", buf);
	}

	sprintf(buf, "__QUIT__NOW__\n");

	if (send_info(&ch, buf, strlen(buf)) < 0) {
		perror("Unable to notify server of client disconnect");
	}

	printf("*** Goodbye\n");

	disconnect_from(&ch);

	return 0;
}
