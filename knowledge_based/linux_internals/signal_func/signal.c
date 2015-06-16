/* This is an example of a possible implementation of the obsolescent signal(2) function.
 *
 * Unreliable signals can be implemented using POSIX's reliable signals interface.
 * In this example, we choose to automatically restart interrupted syscalls, except
 * for SIGALRM. This is common, to make it possible for application code to rely on alarm(2)
 * for I/O timeout. This behavior is platform-dependent though.
 *
 * Note that this implementation of signal(2) shows everything that is wrong with the unreliable
 * signals interface:
 *
 * - We can't check the disposition of a signal without changing it.
 * - We do not control whether interrupted syscalls are automatically restarted. Furthermore,
 *   in this implementation, since we chose not to restart only on SIGALRM, code that wants to
 *   restart interrupted syscalls even with SIGALRM has no way to do so. Similarly, there's no way
 *   to have interrupted syscalls automatically restarted on signals other than SIGALRM.
 * - The signal is not blocked when entering the handler. User code has no way to control this.
 * - The inteface opens the window for race conditions: there is no way to atomically set a signal
 *   handler and block other signals; there's no way to specify that the handler is persistent
 *   across signal occurrences (leading to another race condition between the time that the handler
 *   is entered and the time that it is set again); there's no way to atomically set a signal
 *   handler and pause().
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void (*my_signal(int signo, void (*handler)(int)))(int) {
	struct sigaction old_handler, new_handler;

	new_handler.sa_handler = handler;
	sigemptyset(&new_handler.sa_mask);
	new_handler.sa_flags = 0;

	if (signo == SIGALRM) {
		#ifdef SA_INTERRUPT
		new_handler.sa_flags |= SA_INTERRUPT;
		#endif
	} else {
		new_handler.sa_flags |= SA_RESTART;
	}

	new_handler.sa_flags |= SA_RESETHAND;
	new_handler.sa_flags |= SA_NODEFER;

	if (sigaction(signo, &new_handler, &old_handler) < 0) {
		return SIG_ERR;
	}

	return old_handler.sa_handler;
}

void sigquit(int signo) {
	my_signal(signo, sigquit);
	printf("Caught SIGQUIT\n");
}

int main(void) {
	if (my_signal(SIGQUIT, sigquit) < 0) {
		perror("Couldn't set handler for SIGQUIT");
		exit(EXIT_FAILURE);
	}
	printf("Set handler for SIGQUIT\n");
	raise(SIGQUIT);
	return 0;
}
