/* This is how Linux nowadays implements sleep(3): it is just a wrapper around nanosleep(2).
 *
 * nanosleep(2) has the advantage of doing away with SIGALRM. As such, it does not mess around with
 * a process's signal mask and signal handlers. It offers stronger guarantees; for example, it
 * allows user code to use alarm(2) without worrying about sleep(3).
 *
 */

#include <time.h>
#include <stdio.h>
#include <errno.h>

unsigned sleep4(unsigned seconds) {
	struct timespec to_sleep;
	struct timespec unslept;

	to_sleep.tv_sec = seconds;
	to_sleep.tv_nsec = 0;

	if (nanosleep(&to_sleep, &unslept) < 0 && errno != EINTR) {
		return seconds;
	}

	return unslept.tv_sec;
}

int main(void) {
	printf("Going to sleep for 5 seconds.\n");
	unsigned int sleep_left = sleep4(5);
	printf("Woke up after %u seconds.\n", 5-sleep_left);
	return 0;
}
