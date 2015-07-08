#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static volatile sig_atomic_t sig_flag;
static sigset_t old_nousr;
static struct sigaction sig_action_buf;

static void sigusr_handler(int signo) {
	sig_flag = 1;
}

void NOTIFY_INIT(void) {
	sig_flag = 0;

	sig_action_buf.sa_handler = sigusr_handler;
	sigemptyset(&sig_action_buf.sa_mask);
	sig_action_buf.sa_flags = 0;
	sigaction(SIGUSR1, &sig_action_buf, NULL);


	sigset_t usrmask;
	sigemptyset(&usrmask);
	sigaddset(&usrmask, SIGUSR1);

	// Additionally block SIGUSR1
	sigprocmask(SIG_BLOCK, &usrmask, &old_nousr);
	sigdelset(&old_nousr, SIGUSR1);
}

void NOTIFY(pid_t pid) {
	kill(pid, SIGUSR1);
}

void WAIT_NOTIFY(void) {
	while (sig_flag == 0) {
		sigsuspend(&old_nousr);
	}

	sig_flag = 0;
}

static char msg_buff[] = "A message from a process.\n";

static void write_msgs(const char *proc) {
	int i;
	for (i = 0; i < 10; i++) {
		write(STDOUT_FILENO, proc, strlen(proc));
		write(STDOUT_FILENO, msg_buff, sizeof(msg_buff)-1);
		sleep(1);
	}
}

int main(void) {
	printf("Started\n");
	NOTIFY_INIT();

	pid_t f = fork();
	if (f < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		write_msgs("child: ");
		NOTIFY(getppid());
		WAIT_NOTIFY();
		printf("Child exiting\n");
		NOTIFY(getppid());
	} else {
		/* Child goes first */
		WAIT_NOTIFY();
		write_msgs("parent: ");
		printf("Press any key to terminate\n");
		getchar();
		NOTIFY(f);
		WAIT_NOTIFY();
		printf("Parent exiting\n");
	}

	return 0;
}
