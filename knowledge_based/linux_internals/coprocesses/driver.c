
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define CPROC_FILENAME "cproc"

static int driver_to_cproc[2];
static int cproc_to_driver[2];

void main_loop(void) {
	static char input_buff[32];
	static char output_buff[32];

	while (fgets(input_buff, sizeof(input_buff), stdin) != NULL) {
		size_t len;
		ssize_t written;
		ssize_t readn;

		len = strlen(input_buff);

		if ((written = write(driver_to_cproc[1], input_buff, len)) != len) {
			if (written >= 0)
				fprintf(stderr, "Partial write detected (%zd/%zd)\n", written, readn);
			else
				perror("write(2) error when sending data to coprocess");
			exit(EXIT_FAILURE);
		}

		if ((readn = read(cproc_to_driver[0], output_buff, sizeof(output_buff))) <= 0) {
			if (readn < 0)
				perror("Error retrieving results from coprocess");
			else
				fprintf(stderr, "Child terminated prematurely\n");
			exit(EXIT_FAILURE);
		}

		output_buff[readn] = '\0';
		printf("%s", output_buff);
	}
}

int main(void) {

	if (pipe(driver_to_cproc) < 0 || pipe(cproc_to_driver) < 0) {
		perror("pipe(2) error");
		exit(EXIT_FAILURE);
	}

	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork(2) error");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		if (close(driver_to_cproc[1]) < 0) {
			perror("Error closing driver to coprocess write channel in coprocess");
			exit(EXIT_FAILURE);
		}
		if (close(cproc_to_driver[0]) < 0) {
			perror("Error closing coprocess to driver read channel in coprocess");
			exit(EXIT_FAILURE);
		}

		if (dup2(driver_to_cproc[0], STDIN_FILENO) < 0) {
			perror("Error duplicating driver to coprocess read channel into stdin in coprocess");
			exit(EXIT_FAILURE);
		}
		if (driver_to_cproc[0] != STDIN_FILENO) {
			if (close(driver_to_cproc[0]) < 0) {
				perror("close(2) error when attempting to close already-duplicated read channel");
				exit(EXIT_FAILURE);
			}
		}

		if (dup2(cproc_to_driver[1], STDOUT_FILENO) < 0) {
			perror("Error duplicating coprocess to driver write channel into stdout in coprocess");
			exit(EXIT_FAILURE);
		}
		if (cproc_to_driver[1] != STDOUT_FILENO) {
			if (close(cproc_to_driver[1]) < 0) {
				perror("close(2) error when attempting to close already-duplicated write channel");
				exit(EXIT_FAILURE);
			}
		}

		if (execl(CPROC_FILENAME, CPROC_FILENAME, NULL) < 0) {
			perror("exec error");
			exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
	} else {
		if (close(driver_to_cproc[0]) < 0) {
			perror("Error closing driver to coprocess read channel in driver");
			exit(EXIT_FAILURE);
		}
		if (close(cproc_to_driver[1]) < 0) {
			perror("Error closing driver to coprocess write channel in driver");
			exit(EXIT_FAILURE);
		}
		/* We ignore SIGPIPE because if the child dies for some reason between the time we
		 * wait for input and write it to the pipe, then write(2) will return EPIPE and we
		 * simply terminate.
		 */
		struct sigaction sigact;
		sigemptyset(&sigact.sa_mask);
		sigact.sa_flags = 0;
		sigact.sa_handler = SIG_IGN;

		if (sigaction(SIGPIPE, &sigact, NULL) < 0) {
			perror("Couldn't ignore SIGPIPE");
			exit(EXIT_FAILURE);
		}

		main_loop();
	}

	return 0;
}
