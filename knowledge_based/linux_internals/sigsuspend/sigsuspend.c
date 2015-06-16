/* Describe the differences between sigsuspend(2) and pause(2). What is the need for sigsuspend(2)?
 * Why can't we just set up a handler and call pause(2)?
 *
 * Illustrate your explanation with a code sample.
 *
 * Source: Adapted from Advanced Programming in the UNIX Environment, 3rd ed. (Chapter 10 - Signals)
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* pause(2) puts the process to sleep until a signal is delivered that either terminates the process
 * or causes the invocation of a signal-catching function, whereas sigsuspend(2) atomically sets a
 * process's signal mask and sleeps until a signal is delivered that either terminates the process
 * or causes the invocation of a signal-catching function.
 *
 * The difference is subtle, but important. sigsuspend(2) avoids race conditions between unblocking
 * a signal and calling pause(2). Consider this code sample:
 *
 * ...
 * sigset_t unblock;
 * sigemptyset(&unblock);
 * sigprocmask(SIG_SETMASK, &unblock, NULL); // Unblock every signal
 * pause(); // Sleep until a signal is delivered
 *
 * The problem with the above snippet is that there is a window of time between the call to
 * sigprocmask(2) and the call to pause(2) where the signal may be raised and delivered.
 * Assuming that the signal will never be raised again, pause(2) will sleep forever, whereas
 * in reality the signal already occurred. This is one of those examples where the code seems right
 * and mostly works, but sometimes things go wrong. Problems like this are very hard to debug.
 *
 * sigsuspend(2) solves this problem by atomically changing the signal mask and waiting for
 * a signal. sigsuspend(2) also has the added benefit of restoring the signal mask once it returns.
 *
 * The code below shows a case where pause(2) misses a signal. To see it running, compile without
 * -DUSE_SIGSUSPEND. It works by blocking SIGINT and setting up a handler for SIGINT, followed by
 * raise(SIGINT), which causes SIGINT to be generated for the running process. Then, the original
 * signal mask is restored, unblocking SIGINT. sigprocmask(2) returns only after pending signals
 * are delivered and their handlers executed, so by the time pause(2) is called, the signal has
 * already been lost. The process sleeps forever.
 *
 * Compiling with -DUSE_SIGSUSPEND will illustrate the use of sigsuspend(2). Because sigsuspend(2)
 * atomically sets the signal mask and waits for a signal, the process does not sleep forever
 * anymore.
 */

void sigint(int signo) {
	printf("Caught SIGINT\n");
}

#ifndef USE_SIGSUSPEND

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

#else

int main(void) {
	sigset_t block, old, sigmask;

	sigemptyset(&sigmask);

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

	// Atomically unblock SIGINT and pause
	printf("Calling sigsuspend()\n");
	sigsuspend(&sigmask);

	if (sigprocmask(SIG_SETMASK, &old, NULL) < 0) {
		perror("Couldn't restore original signal mask");
		exit(EXIT_FAILURE);
	}

	printf("Restored signal mask\n");

	return 0;
}

#endif /* USE_SIGSUSPEND */
