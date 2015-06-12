/* How does the time(1) command work? What is the meaning of each time value? */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s command [arguments]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct timespec rtime_begin, rtime_end;
	if (clock_gettime(CLOCK_REALTIME, &rtime_begin) < 0) {
		perror("Error measuring wall clock time");
		exit(EXIT_FAILURE);
	}

	pid_t f;
	if ((f = fork()) < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		if (execvp(argv[1], &argv[1]) < 0) {
			perror("exec error");
			exit(EXIT_FAILURE);
		}
	}

	/* We ignore SIGINT so that times are still printed if the user
	 * interrupts a long process
	 */

	struct sigaction sigint_ign;
	sigint_ign.sa_handler = SIG_IGN;
	sigemptyset(&sigint_ign.sa_mask);
	sigint_ign.sa_flags = 0;

	if (sigaction(SIGINT, &sigint_ign, NULL) < 0) {
		perror("Couldn't ignore SIGINT");
		exit(EXIT_FAILURE);
	}

	int status;
	struct rusage rusage;

	if (wait3(&status, 0, &rusage) == (pid_t) -1) {
		perror("wait error");
		exit(EXIT_FAILURE);
	}

	if (clock_gettime(CLOCK_REALTIME, &rtime_end) < 0) {
		perror("Error measuring wall clock time");
		exit(EXIT_FAILURE);
	}

	double real, user, sys;

	real = (rtime_end.tv_sec-rtime_begin.tv_sec)+(rtime_end.tv_nsec-rtime_begin.tv_nsec)/1000000000.0;
	user = rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec/1000000.0;
	sys = rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec/1000000.0;

	fprintf(stderr, "\nreal\t%.4f secs.\nuser\t%.4f secs.\nsys\t%.4f secs.\n", real, user, sys);

	return 0;
}
