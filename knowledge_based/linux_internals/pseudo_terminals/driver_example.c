/* A simple hello world example of a driver program for pty.
 *
 * A driver program's stdin is redirected to the read channel of a pipe that is
 * fed by the pty parent. The pty parent reads from the master pseudo-terminal
 * device and writes to the pipe.
 *
 * A driver program's stdout is redirected to the write channel of a pipe that
 * is read by the pty child. The pty child reads from the pipe and writes to
 * the pseudo-terminal master device.
 *
 * For this particular example program, we recommend testing it with cat(1), as
 * in:
 *
 * ./pty -d ./driver cat
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	printf("Hello\n");
	printf("World!\n");
	printf("Goodbye\n");
	printf("World!\n");

	/* We need to close stdout to let the pty child know that there will be
	 * no more input
	 */
	fclose(stdout);

	static char buff[2048];

	ssize_t n;
	while ((n = read(STDIN_FILENO, buff, sizeof(buff))) > 0) {
		write(STDERR_FILENO, buff, n);
	}

	if (n < 0) {
		perror("read(2) error");
		return 1;
	}

	return 0;
}
