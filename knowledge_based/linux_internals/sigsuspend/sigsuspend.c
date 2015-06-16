/* Describe the differences between sigsuspend(2) and pause(2). What is the need for sigsuspend(2)?
 * Why can't we just set up a handler and call pause(2)?
 * Illustrate your explanation with a code sample.
 *
 * Source: Adapted from Advanced Programming in the UNIX Environment, 3rd ed. (Chapter 10 - Signals)
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void sigint(int signo) {
	printf("Caught SIGINT\n");
}

int main(void) {
	sigset_t block, old;

	sigemptyset(&block);
	sigaddset(&block, SIGINT);
	if (sigprocmask(SIG_SETMASK, &block, &old) < 0) {
		perror("Couldn't block SIGINT");
		exit(EXIT_FAILURE);
	}

	printf("SIGINT blocked\n");
	raise(SIGINT);
	printf("SIGINT now pending\n");

	struct sigaction sigint_act;
	sigint_act.sa_handler = sigint;
	sigemptyset(&sigint_act.sa_mask);
	sigint_act.sa_flags = 0;

	if (sigaction(SIGINT, &sigint_act, NULL) < 0) {
		perror("Couldn't set handler for SIGINT");
		exit(EXIT_FAILURE);
	}

	if (sigprocmask(SIG_SETMASK, &old, NULL) < 0) {
		perror("Couldn't restore original signal mask");
		exit(EXIT_FAILURE);
	}

	printf("Calling pause()...\n");

	// A naive programmer would expect this to return immediately,
	// but the pending SIGINT was already delivered. We lost it!
	pause();

	return 0;
}
