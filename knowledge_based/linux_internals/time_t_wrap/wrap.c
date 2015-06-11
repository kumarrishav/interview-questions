/* Calculate the latest time that can be represented by the time_t data type. */
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* NOTE:
 * This code will fail on 64-bit systems because the year does not fit into an integer
 * in `struct tm`, so gmtime(3) is unable to build a broken down time out of time_t,
 * which is good, because 64-bit time_t's will not wrap around in 2038! Yay!
 *
 * Older systems will have 32-bit time_t's and this will work. In such systems, time_t
 * will wrap around on 01/19/38 03:14:07 +0000 UTC
 *
 * To emulate 32-bit behavior in a 64-bit system, replace this line:
 *
 * time_t max_time = ~(((time_t) 1) << (sizeof(time_t)*CHAR_BIT-1));
 *
 * with:
 *
 * time_t max_time = 2147483647;
 *
 * Fun fact, taken from Expert C Programming - Deep C Secrets:
 * 
 * The original bugs section for the manpage of tunefs in 4.2 BSD said:
 *
 * You can tune a filesystem, but you can't tune a fish.
 *
 * According to Noah Friedman on alt.folklore.computers on 18 Mar 1992, he hinted that the
 * "tune a fish" sentence was removed in SunOS 4.1.0 and subsequently reinserted in SunOS 4.1.1.
 * SunOS 4, completely Berkeley based, was replaced with the SVR4 based SunOS 5,
 * and the tunefs manpage no longer included this text. However, Friedman indicated that the
 * reinsertion into 4.1.1 appeared as the following raw troff in the tunefs manpage:
 *
 * Take this out and a Unix Demon will dog your steps from now until
 * the time_t's wrap around.
 *
 * Cute, huh?
 */

int main(void) {
	static char date_buf[64];
	time_t max_time = ~(((time_t) 1) << (sizeof(time_t)*CHAR_BIT-1));
	struct tm broken_down;

	if (gmtime_r(&max_time, &broken_down) == NULL) {
		fprintf(stderr, "gmtime() error: year does not fit into an integer (is time_t 64 bits wide?)\n");
		exit(EXIT_FAILURE);
	}

	strftime(date_buf, sizeof(date_buf), "%D %I:%M:%S %z %Z", &broken_down);

	printf("time_t will wrap around on %s\n", date_buf);
	return 0;
}
