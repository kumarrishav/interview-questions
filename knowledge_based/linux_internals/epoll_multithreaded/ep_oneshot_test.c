/* This program was created to test an edge case in how epoll manages multithreaded programs.
 * It turns out it manages them quite well.
 *
 * It all started with this question on StackOverflow:
 * http://stackoverflow.com/questions/31531823/thread-safety-of-epoll
 *
 * Basically, the question is what happens if another thread rearms a file descriptor that is
 * already readable when another thread is already blocked on epoll_wait(2).
 *
 * This program shows that the behavior is well defined: epoll_wait(2) will fire an EPOLLIN event
 * for that descriptor as soon as it is rearmed.
 *
 * This is subtly documented in the Q/A section of man 7 epoll:
 *
 * Q8 Does an operation on a file descriptor affect the already collected but not yet reported
 *    events?
 *
 * A8 You can do two operations on an existing file descriptor. Remove would be meaningless for this
 *    case. Modify will reread available I/O.
 *
 */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <semaphore.h>
#include <sys/epoll.h>
#include <unistd.h>

static pthread_t tids[2];
static int epoll_fd;
static char input_buff[512];
static sem_t chunks_sem;

void *dispatcher(void *arg) {
	struct epoll_event epevent;

	while (1) {
		printf("Dispatcher waiting for more chunks\n");
		if (epoll_wait(epoll_fd, &epevent, 1, -1) < 0) {
			perror("epoll_wait(2) error");
			exit(EXIT_FAILURE);
		}

		ssize_t n;
		if ((n = read(STDIN_FILENO, input_buff, sizeof(input_buff)-1)) <= 0) {
			if (n < 0)
				perror("read(2) error");
			else
				fprintf(stderr, "stdin closed prematurely\n");
			exit(EXIT_FAILURE);
		}

		input_buff[n] = '\0';
		sem_post(&chunks_sem);
	}

	return NULL;
}

void *consumer(void *arg) {
	sigset_t smask;
	sigemptyset(&smask);
	sigaddset(&smask, SIGUSR1);

	while (1) {
		sem_wait(&chunks_sem);
		printf("Consumer received chunk: %s", input_buff);
		/* Simulate some processing... */
		sleep(2);
		printf("Consumer finished processing chunk.\n");
		printf("Please send SIGUSR1 after sending more data to stdin\n");

		int signo;
		if (sigwait(&smask, &signo) < 0) {
			perror("sigwait(3) error");
			exit(EXIT_FAILURE);
		}

		assert(signo == SIGUSR1);

		struct epoll_event epevent;
		epevent.events = EPOLLIN | EPOLLONESHOT;
		epevent.data.fd = STDIN_FILENO;

		if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, STDIN_FILENO, &epevent) < 0) {
			perror("epoll_ctl(2) error when attempting to readd stdin");
			exit(EXIT_FAILURE);
		}

		printf("Rearmed stdin\n");
	}
}

int main(void) {

	sigset_t sigmask;
	sigfillset(&sigmask);
	if (pthread_sigmask(SIG_SETMASK, &sigmask, NULL) < 0) {
		perror("pthread_sigmask(3) error");
		exit(EXIT_FAILURE);
	}

	if ((epoll_fd = epoll_create(1)) < 0) {
		perror("epoll_create(2) error");
		exit(EXIT_FAILURE);
	}

	struct epoll_event epevent;
	epevent.events = EPOLLIN | EPOLLONESHOT;
	epevent.data.fd = STDIN_FILENO;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &epevent) < 0) {
		perror("epoll_ctl(2) error");
		exit(EXIT_FAILURE);
	}

	if (sem_init(&chunks_sem, 0, 0) < 0) {
		perror("sem_init(3) error");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&tids[0], NULL, dispatcher, NULL) < 0) {
		perror("pthread_create(3) error on dispatcher");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&tids[1], NULL, consumer, NULL) < 0) {
		perror("pthread_create(3) error on consumer");
		exit(EXIT_FAILURE);
	}

	size_t i;
	for (i = 0; i < sizeof(tids)/sizeof(tids[0]); i++) {
		if (pthread_join(tids[i], NULL) < 0) {
			perror("pthread_join(3) error");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
