/* This code tests whether the system queues pending signals */

#include <stdio.h>
#include <signal.h>

void sigquit(int signo) {
	if (signal(SIGQUIT, sigquit) == SIG_ERR) {
		perror("Unable to set signal handler");
	}
	printf("SIGQUIT caught\n");
}

int main(void) {
	sigset_t oldmask, newmask;

	if (sigemptyset(&newmask) < 0) {
		perror("Couldn't initialize newmask");
		return 0;
	}

	if (sigaddset(&newmask, SIGQUIT) < 0) {
		perror("Couldn't add SIGQUIT to newmask");
		return 0;
	}

	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
		perror("sigprocmask() error");
		return 0;
	}

	if (signal(SIGQUIT, sigquit) == SIG_ERR) {
		perror("Unable to set signal handler");
		return 0;
	}

	/* SIGQUIT is now blocked */

	int i;
	for (i = 0; i < 10; i++) {
		raise(SIGQUIT);
	}

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
		perror("Unable to restore signal mask");
	}

	return 0;
}
