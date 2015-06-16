
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
