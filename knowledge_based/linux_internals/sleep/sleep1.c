/* A simple, yet highly unreliable implementation of sleep(3).
 *
 * First and foremost, this implementation uses signal(2).
 *
 * The exact semantics of signal(2) are platform-dependent, which hinders code portability.
 * For example, the behavior of signal(2) with interrupted syscalls varies among platforms.
 *
 * Perhaps even worse than that, signal(2) is obsolescent due to its unreliable semantics:
 * - We can't simply check a signal's disposition without changing it
 * - When a signal is caught, the default action is restarted. We can always set the handler again
 *   once we catch the signal, but there is a window of time between entering the handler and
 *   calling signal(2) where the signal could be raised again, without having set the correct
 *   handler. Thus, a process that always catches SIGTERM can still end up being terminated
 *   unproperly.
 * - There is no way to block signals for later delivery. Sometimes, critical code regions need
 *   to block a certain signal from being raised, while still remembering if that signal occurred
 *   for later delivery. This is not possible with signal(2).
 *
 * Further problems with this version of sleep(3) include:
 * - A race condition between alarm(2) and pause(2). It is possible, though unlikely in most cases,
 *   that the specified amount of seconds elapses before the call to pause(2), which might put the
 *   process to sleep forever.
 * - It changes the disposition of SIGALRM. We should save the previous action and restore it before
 *   returning.
 * - If there was an active alarm(2) in progress from user code, that alarm is canceled. If an alarm
 *   is in progress, we should wait only that amount of time.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void sigalrm(int signo) {
	/* Nothing to do here */
}

unsigned int sleep1(unsigned int seconds) {
	if (signal(SIGALRM, sigalrm) == SIG_ERR) {
		return seconds;
	}
	alarm(seconds);
	pause();
	return alarm(0);
}

int main(void) {
	printf("Going to sleep for 5 seconds.\n");
	unsigned int sleep_left = sleep1(5);
	printf("Woke up after %u seconds.\n", 5-sleep_left);
	return 0;
}
