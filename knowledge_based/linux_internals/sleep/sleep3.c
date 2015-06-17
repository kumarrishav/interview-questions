/* This code implements a reliable, POSIX compliant sleep(3).
 *
 * It addresses every issue in sleep1.c and sleep2.c:
 *
 * - It doesn't destroy other signal handlers execution because it doesn't use setjmp/longjmp
 * - There's no race condition between the call to alarm(2) and the signal waiting time, because
 *   it uses sigsuspend(2) to atomically unblock SIGALRM and wait for it. Furthermore, it blocks
 *   SIGALRM prior to calling alarm(2).
 * - It restores the process's original signal mask and SIGALRM disposition upon exiting
 *
 * POSIX leaves up to the implementation to define what happens when sleep(3) is called in a
 * situation where alarm(2) has been called by user code before. In this implementation,
 * that alarm is cleared and overriden by our sleep. The manpage for sleep(3) explicitly says
 * that it is not a good idea to play with sleep(3) and alarm(2) at the same time.
 *
 * Finally, it is worth mentioning that Linux these days implements sleep(3) with nanosleep(2).
 * nanosleep(2) does away with SIGALRM and signal handling altogether, and is a much better choice
 * because it doesn't mess with signals at all.
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sigalrm_sleep(int signo) {
	/* Nothing to do here */
}

unsigned sleep3(unsigned int seconds) {
	/* Step 1 - Block SIGALRM */
	sigset_t blockmask, originalmask, mask_noalrm;
	sigemptyset(&blockmask);
	sigaddset(&blockmask, SIGALRM);
	if (sigprocmask(SIG_BLOCK, &blockmask, &originalmask) < 0) {
		return seconds;
	}

	/* Step 2 - set up our own signal handler for SIGALRM */
	struct sigaction sigalrm_act, sigalrm_orig;
	sigalrm_act.sa_handler = sigalrm_sleep;
	sigemptyset(&sigalrm_act.sa_mask);
	sigalrm_act.sa_flags = 0;
	if (sigaction(SIGALRM, &sigalrm_act, &sigalrm_orig) < 0) {
		sigprocmask(SIG_SETMASK, &originalmask, NULL);
		return seconds;
	}

	/* Step 3 - Atomically unblock SIGALRM and wait for it */
	mask_noalrm = originalmask;
	sigdelset(&mask_noalrm, SIGALRM);
	alarm(seconds);
	/* No race condition here because SIGALRM is still blocked */
	sigsuspend(&mask_noalrm);

	unsigned unslept = alarm(0);

	/* Step 4 - Restore handler */
	sigaction(SIGALRM, &sigalrm_orig, NULL);
	/* Step 5 - Restore signal mask */
	sigprocmask(SIG_SETMASK, &originalmask, NULL);

	return unslept;
}

int main(void) {
	printf("Going to sleep for 5 seconds.\n");
	unsigned int sleep_left = sleep3(5);
	printf("Woke up after %u seconds.\n", 5-sleep_left);
	return 0;
}
