/* This implementation still uses signal(3). To see why it is a bad idea to use signal(3), refer to
 * the description in sleep1.c
 *
 * On the other hand, this version solves all of the problems of the previous version (but
 * unfortunately creates new problems):
 * - It restores SIGALRM's disposition to whatever it was when the function was entered
 * - If an alarm is pending that will expire earlier than the specified number of seconds,
 *   then it only waits until the early alarm is triggered
 * - It uses longjmp and setjmp to avoid the race condition between setting the signal handler
 *   and pausing.
 *
 * However, the use of setjmp and longjmp brings a new problem when dealing with other signals.
 * Consider what happens if another signal handler is in the midst of execution when the alarm is
 * triggered. Since SIGALRM's handler uses longjmp, it will cancel the other signal handler that is
 * executing. This is not desirable at all.
 *
 * sleep1() and sleep2() are examples of code that seems correct and works most of the time, but
 * suffers from those pathological cases. Problems like these are extremely hard to debug.
 * It is interesting to note that code similar to sleep2() was indeed shipped on various UNIX
 * systems, making its way onto production environments. This was the case with SVR2.
 *
 * Try to mess around with signal handling and sleeping at the same time in SVR2, and the results
 * can be surprising.
 *
 * sleep1() and sleep2() show the consequences of dealing with signal handling naively.
 */
#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static jmp_buf jmpbuf;

void sigalrm(int signo) {
	longjmp(jmpbuf, 1);
}

unsigned int sleep2(unsigned int seconds) {
	void (*prev_handler)(int) = signal(SIGALRM, sigalrm);
	if (prev_handler == SIG_ERR) {
		return seconds;
	}

	if (setjmp(jmpbuf) == 0) {
		unsigned secs = alarm(seconds);
		if (secs != 0 && secs < seconds) {
			alarm(secs);
		} else {
			secs = seconds;
		}
		pause();
	}

	signal(SIGALRM, prev_handler);

	return alarm(0);
}

int main(void) {
	printf("Going to sleep for 5 seconds.\n");
	unsigned int sleep_left = sleep2(5);
	printf("Woke up after %u seconds.\n", 5-sleep_left);
	return 0;
}
