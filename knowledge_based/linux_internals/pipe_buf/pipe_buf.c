
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int main(void) {
	int fildes[2];

	if (pipe(fildes) < 0) {
		perror("pipe(2) failed");
		exit(EXIT_FAILURE);
	}

	if (fcntl(fildes[1], F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl(2) failed");
		exit(EXIT_FAILURE);
	}

	printf("PIPE_BUF = %d\n", PIPE_BUF);

	int i;
	for (i = 0; write(fildes[1], "a", 1) != -1; i++)
		; /* Intentionally left blank */

	if (errno == EAGAIN || errno == EWOULDBLOCK) {
		printf("write(2) would block (%d bytes written)\n", i);
	} else {
		perror("write(2) failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}
