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

/* The code below makes use of a struct timespec to represent time. UNIX platforms have quite a
 * lot of library functions and syscalls that deal with time, and there are a dozen structs
 * out there to represent time:
 *
 * struct timeval
 * struct timespec
 * struct tm
 * time_t
 *
 * There are also a dozen of functions to get the system time. Why is this such a complicated
 * topic?
 * 
 * This is a good opportunity to review the different time functions and types in the UNIX
 * environment. The average programmer is often bewildered by the different amount of
 * structs used to represent time. It is easy to make a big confusion out of this.
 *
 * Internally, the kernel always represents time as the number of seconds elapsed since
 * January 1st, 1970, 00:00:00 +0000 UTC (Coordinated Universal Time). This is called the Epoch.
 *
 * The type used to store this number of seconds is `time_t`.
 *
 * Now, there are 3 functions that we can use to get the current time as a `time_t`. All 3 of these
 * are syscalls:
 *
 * - gettimeofday(struct timeval *, struct timezone *)
 *   First of all, the only portable use of the 2nd argument is a NULL pointer. The semantics vary
 *   across platforms, so it is generally not a good idea to use it anyway. This function returns
 *   the Epoch time in a struct timeval. A struct timeval has microsecond granularity:
 *
 *       struct timeval {
 *           time_t tv_sec;
 *           suseconds_t tv_usec;
 *       };
 *
 * As of the Single UNIX Specification Version 4, gettimeofday(2) is obsolescent; it should be
 * allowed to rest in peace unless its use is required for backward compatibility.
 *
 * - time(time_t *)
 *   This returns the Epoch time as a raw `time_t`. It has less granularity than gettimeofday(2),
 *   but it's not obsolescent. It's a pretty basic interface that is simple and works well.
 *   This is all we need most of the time.
 *
 * - clock_gettime(clockid_t, struct timespec *)
 *   This is the successor of gettimeofday(2) after real-time extensions made it to POSIX base.
 *   Real-time extensions specify that a system has multiple clocks; the first argument indicates
 *   which clock we want to extract the time from. Refer to the manpage for clock_gettime(2) for
 *   a list of available clocks. Most of the times, CLOCK_REALTIME is enough.
 *   The function returns the Epoch time in a struct timespec, which differs from struct timeval in
 *   that is has nanosecond (rather than microsecond) granularity. It is very similar to a timeval:
 *
 *   struct timespec {
 *       time_t tv_sec;
 *       long tv_nsec;
 *   };
 *
 * These are the 3 ways to get the time from the kernel.
 *
 * Library functions are used to transform this time into a broken down time (which is of
 * type struct tm). A broken down time is a step towards human-readable time; instead of
 * the number of seconds since the Epoch, it includes the day, month, year, week, weekday,
 * etc. A broken down time is built off of a time_t with gmtime(3) or localtime(3). localtime(3)
 * takes into account the user's timezone; gmtime(3) doesn;t.
 *
 * Finally, we can turn a broken down time (struct tm) into a human-readable string with
 * strftime(3).
 *
 * We can also turn a human-readable string into a broken down time with strptime(3), and from
 * there we can turn it into a time_t with mktime(3).
 *
 * It's not so hard when we know what we're talking about, is it? ;)
 *
 */

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
