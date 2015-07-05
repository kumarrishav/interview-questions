/* Determine the capacity of a pipe using nonblocking writes. Compare this value with the
 * value of PIPE_BUF from Chapter 2.
 *
 * Source: Advanced Programming in the UNIX Environment, 3rd Ed., Chapter 14, Exercise 14.7
 */
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

/* This test creates a pipe, sets the write end non blocking, and writes into the pipe until it
 * is not possible to write anymore without blocking.
 *
 * In Linux, the pipe capacity is 65536 bytes. This is the amount of bytes successfully written
 * before write(2) returns EAGAIN or EWOULDBLOCK.
 *
 * In constrast, PIPE_BUF is 4096, a much smaller value. Note that PIPE_BUF is the largest value
 * that is atomically written into the pipe. This is not necessarily the pipe capacity, which is
 * generally greater, as can be proven by this example.
 *
 */

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
