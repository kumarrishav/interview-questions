/* Write a program to illustrate what happens with orphaned process groups. */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sighup(int signo) {
	printf("SIGHUP received; pid = %ld\n", (long) getpid());
}

void print_ids(const char *process) {
	printf("%s: pid = %ld, ppid = %ld, pgrp = %ld, tpgrp = %ld\n",
	       process, (long) getpid(), (long) getppid(), (long) getpgrp(),
	       (long) tcgetpgrp(STDIN_FILENO));
}

int main(void) {
	print_ids("parent");
	pid_t f = fork();

	if (f < 0) {
		perror("Couldn't fork");
		return 0;
	}

	if (f == 0) {
		print_ids("child");
		signal(SIGHUP, sighup);
		kill(getpid(), SIGTSTP);
		print_ids("child");

		char c;
		if (read(STDIN_FILENO, &c, 1) != 1) {
			perror("Couldn't read from stdin");
		}

	} else {
		sleep(5);
	}

	return 0;
}
