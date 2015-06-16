/* An implementation of abort(3)
 *
 * Based on the ideas exposed in Advanced Programming in the UNIX Environment, 3rd ed.
 * Section 10.17
 *
 * abort(3) aborts the current process by raising SIGABRT. It is implementation-defined whether
 * output streams are flushed. This implementation flushes output streams.
 *
 * If the caller ignores or is currently blocking SIGABRT, abort(3) overrides this behavior by
 * unblocking the signal and setting the action to SIG_DFL.
 *
 * If the caller is catching the signal and the signal handler returns, abort(3) shall terminate
 * the process. In other words, abort(3) never returns.
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void abort(void) {
	struct sigaction action;
	sigaction(SIGABRT, NULL, &action);

	/* Override SIG_IGN */
	if (action.sa_handler == SIG_IGN) {
		action.sa_handler = SIG_DFL;
		sigaction(SIGABRT, &action, NULL);
	}

	/* Flush all output streams */
	fflush(NULL);

	/* Override blocked SIGABRT */
	sigset_t unblock;
	sigfillset(&unblock);
	sigdelset(&unblock, SIGABRT);
	sigprocmask(SIG_SETMASK, &unblock, NULL);

	raise(SIGABRT);

	/* If we get here, process caught signal and the handler returned
	 * New output may have been generated; flush streams again
	 */
	fflush(NULL);

	/* Time to do away with signal handlers */
	action.sa_handler = SIG_DFL;
	sigaction(SIGABRT, &action, NULL);

	/* Signal handler might have blocked SIGABRT */
	sigprocmask(SIG_SETMASK, &unblock, NULL);

	/* Good bye */
	raise(SIGABRT);

	/* This is never reached
	 * We just need it because of this:
	 * abort.c:56:1: warning: ‘noreturn’ function does return [enabled by default]
	 */
	exit(1);
}

int main(void) {
	struct sigaction act;

	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	if (sigaction(SIGABRT, &act, NULL) < 0) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	printf("Ignored SIGABRT\n");
	printf("Calling abort()\n");
	abort();

	/* Not reached */
	return 0;
}
