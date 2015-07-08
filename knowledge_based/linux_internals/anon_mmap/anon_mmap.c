/* This program illustrates anonymous memory mapping as an IPC mechanism.
 *
 * mmap(2) with the MAP_ANONYMOUS flag is a good way to share memory between processes.
 * Note that MAP_ANONYMOUS is not POSIX and as such implementations are not required to support it.
 * For maximum portability, we could open /dev/zero instead.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>

#define NLOOPS 100

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Parent / Child synchronization primitives
 * See knowledge_based/linux_internals/fork_sync for more info
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
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
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

static long *counter;

void update(void) {
	(*counter)++;
}

int main(void) {
	counter = mmap(NULL, sizeof(long),
		       PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
		       -1, 0);

	if (counter == MAP_FAILED) {
		perror("mmap(2) failed");
		exit(EXIT_FAILURE);
	}

	NOTIFY_INIT();
	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork(2) failed");
		exit(EXIT_FAILURE);
	}

	int i;
	if (pid == 0) {
		for (i = 0; i < NLOOPS; i++) {
			update();
			printf("%ld: counter = %ld\n", (long) getpid(), *counter);
			NOTIFY(getppid());
			WAIT_NOTIFY();
		}
	} else {
		for (i = 0; i < NLOOPS; i++) {
			WAIT_NOTIFY();
			update();
			printf("%ld: counter = %ld\n", (long) getpid(), *counter);
			NOTIFY(pid);
		}
	}

	return 0;
}
