/* Write a test program that illustrates your system's behavior when a process is blocked
 * while trying to write lock a range of a file and additional read-lock requests are made.
 * Is the process requesting a write lock starved by the processes read locking the file?
 *
 * Source: Advanced Programming in the UNIX Environment, 3rd Edition, Chapter 14, Exercise 14.1
 */

/* This was tested in Linux, where a writer process is blatantly starved as long as readers exist.
 * This is suboptimal, to say the least. Combine this with the buggy mandatory locking
 * implementation in Linux that has race conditions all over the place. It's about time someone
 * improves this on Linux.
 */

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEST_CHILDREN 30
#define FILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* Parent / child synchronization mechanism using SIGUSR1
 * See knowledge_based/linux_internals/fork_sync/ for more info
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

int read_lock(int fd) {
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	return fcntl(fd, F_SETLKW, &lock);
}

int write_lock(int fd) {
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	return fcntl(fd, F_SETLKW, &lock);
}

int main(int argc, char *argv[]) {

	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd;
	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILEMODE)) < 0) {
		perror("open(2) failed");
		exit(EXIT_FAILURE);
	}

	if (read_lock(fd) < 0) {
		perror("Couldn't lock file for reading");
		exit(EXIT_FAILURE);
	}

	pid_t pid;
	if ((pid = fork()) < 0) {
		perror("fork(2) failed");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		printf("Writer started; attempting to acquire write lock\n");
		if (write_lock(fd) < 0) {
			perror("Error while attempting to acquire write lock");
			exit(EXIT_FAILURE);
		}
		printf("Acquired write lock\n");
		exit(EXIT_SUCCESS);
	}

	NOTIFY_INIT();

	int i;
	for (i = 0; i < TEST_CHILDREN; i++) {
		if ((pid = fork()) < 0) {
			perror("fork(2) failed");
			exit(EXIT_FAILURE);
		}

		/* We can only terminate the parent once the child gets a read lock, so that there
		 * is no possible time window where no read locks are acquired; this is a stress
		 * test to see if the writer is left starving
		 */

		if (pid == 0) {

			if (read_lock(fd) < 0) {
				perror("Couldn't lock file for reading");
				exit(EXIT_FAILURE);
			}

			/* Parent is now free to go */
			NOTIFY(getppid());

			printf("Reader %ld acquired read lock\n", (long) getpid());
			sleep(1);
			printf("Reader %ld releasing read lock\n", (long) getpid());

		} else {
			WAIT_NOTIFY();
			exit(EXIT_SUCCESS);
		}
	}

	return 0;
}
