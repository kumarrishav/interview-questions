/* Compile with -DUSE_STDIO to have the coprocess use stdio */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifndef USE_STDIO

int main(void) {
	static char output_buff[32];
	static char input_buff[32];
	static char invalid_arg_err[] = "Invalid arguments.\n";
	size_t invalid_arg_err_sz = sizeof(invalid_arg_err)-1;
	ssize_t n;

	while ((n = read(STDIN_FILENO, input_buff, sizeof(input_buff))) > 0) {
		ssize_t sent;
		int op1, op2;

		if (sscanf(input_buff, "%d%d", &op1, &op2) != 2) {
			if ((sent = write(STDOUT_FILENO, invalid_arg_err, invalid_arg_err_sz)) != invalid_arg_err_sz) {
				if (sent >= 0)
					fprintf(stderr, "Partial write detected when reporting invalid arguments (%zd/%zd)\n",
						sent, invalid_arg_err_sz);
				else
					perror("write(2) error when reporting invalid arguments");
				exit(EXIT_FAILURE);
			}
			continue;
		}

		int len = sprintf(output_buff, "%d\n", op1+op2);
		if ((sent = write(STDOUT_FILENO, output_buff, len)) != len) {
			if (len >= 0)
				fprintf(stderr, "Partial write detected when sending result (%zd/%d)\n",
					sent, len);
			else
				perror("write(2) error when sending result");
		}
	}

	if (n < 0) {
		perror("read(2) error");
		exit(EXIT_FAILURE);
	}

	return 0;
}

#else

int main(void) {
	static char line_buff[32];

	/* We have to change buffering mode for stdin and stdout because we know that this is a
	 * coprocess and a such stdin and stdout will be attached to pipes. stdio switches to fully
	 * buffered mode when reading from and writing to a pipe, which means that buffers are only
	 * flushed when either the file stream is closed or when they are filled.
	 * Thus, if we don't change to line buffered mode, fgets() will block waiting for more input
	 * even after the driver process sends a line, and the driver process will block waiting for
	 * the reply from the coprocess - deadlock!
	 *
	 * A fancy workaround is to use pseudo-terminals instead: the driver process allocates a
	 * controlling terminal and has the coprocess read from and write to it, which makes stdio
	 * enter line-buffered mode again (because now it's behind a terminal device). This is
	 * explored in knowledge_based/linux_internals/pseudo_terminals
	 *
	 */
	if (setvbuf(stdin, NULL, _IOLBF, 0) != 0 || setvbuf(stdout, NULL, _IOLBF, 0) < 0) {
		fprintf(stderr, "setvbuf(3) error: couldn't change to line-buffered mode.\n");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	while (fgets(line_buff, sizeof(line_buff), stdin) != NULL) {
		int op1, op2;
		if (sscanf(line_buff, "%d%d", &op1, &op2) != 2) {
			printf("Invalid arguments.\n");
		} else {
			printf("%d\n", op1+op2);
		}
		errno = 0;
	}

	if (errno != 0) {
		perror("fgets(3) error");
		exit(EXIT_FAILURE);
	}

	return 0;
}

#endif /* USE_STDIO */
