/* Describe the different ways a process can terminate. What is the difference between the
 * termination status and the exit status?
 *
 * How can a parent process know what happened to a child?
 *
 * Source: Adapted from Advanced Programming in the UNIX Enviornment, 3rd Edition
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void pr_exit(int status) {
	if (WIFEXITED(status)) {
		printf("Child terminated normally, exit status = %d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		printf("Terminated with %s", strsignal(WTERMSIG(status)));
		#ifdef WCOREDUMP
		if (WCOREDUMP(status)) {
			printf(" (core dumped)");
		}
		#endif
		printf("\n");
	} else if (WIFSTOPPED(status)) {
		printf("Stopped with %s\n", strsignal(WSTOPSIG(status)));
	}
}

int main(void) {
	pid_t f;

	if ((f = fork()) < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		exit(EXIT_SUCCESS);
	} else {
		int status;
		wait(&status);
		pr_exit(status);
	}

	if ((f = fork()) < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		abort(); /* Generates SIGABRT */
	} else {
		int status;
		wait(&status);
		pr_exit(status);
	}

	if ((f = fork()) < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		int a = 3;
		int b = 0;
		int c = a/b; /* Generates SIGFPE */
		printf("%d\n", c);
	} else {
		int status;
		wait(&status);
		pr_exit(status);
	}

	return 0;
}
