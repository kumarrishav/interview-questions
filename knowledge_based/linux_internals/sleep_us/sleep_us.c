/* Implement the function sleep_us(), which is similar to sleep(3), but waits for a specified
 * number of microseconds. Use either select(2) or poll(2). Compare this function to the BSD
 * usleep(3) function.
 *
 * Source: Advanced Programming in the UNIX Environment 3rd Ed., Chapter 14, Exercise 14.5
 */

/* A simple implementation of usleep(3)
 *
 * usleep(3) sleeps for the specified amount of microseconds,
 * which must be a value between 0 and 1000000.
 *
 * It can return prematurely if interrupted by a signal. If this happens,
 * it returns -1 and sets errno to EINTR.
 *
 * Note that usleep(3) is declared obsolete in POSIX.1-2001; nanosleep(2) should
 * be used instead.
 */ 

#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int sleep_us(unsigned usec) {

	if (!(usec >= 0 && usec <= 1000000)) {
		errno = EINVAL;
		return -1;
	}

	struct timeval timeval;
	timeval.tv_sec = 0;
	timeval.tv_usec = usec;

	return select(0, NULL, NULL, NULL, &timeval);
}

int main(void) {
	printf("Sleeping half a second.\n");
	if (sleep_us(500) < 0) {
		perror("sleep_us() failed");
		exit(EXIT_FAILURE);
	}
	printf("Woke up.\n");
	return 0;
}
