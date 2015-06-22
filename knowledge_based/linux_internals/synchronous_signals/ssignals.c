
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int flag;
sigset_t smask;

void *thread_fn(void *arg) {
	printf("Thread started\n");

	int signal;

	while (1) {
		if (sigwait(&smask, &signal) < 0) {
			perror("sigwait()");
			exit(EXIT_FAILURE);
		}
		switch (signal) {
		case SIGQUIT:
			printf("Got SIGQUIT\n");
			break;
		case SIGTERM:
			printf("Got SIGTERM, preparing to exit\n");
			pthread_mutex_lock(&cond_lock);
			flag = 1;
			pthread_mutex_unlock(&cond_lock);
			pthread_cond_signal(&cond);
			break;
		case SIGINT:
			printf("Got SIGINT\n");
			break;
		default:
			printf("Got unexpected signal %d (%s)\n", signal, strsignal(signal));
			break;
		}
	}

}

int main(void) {

	sigemptyset(&smask);
	sigaddset(&smask, SIGQUIT);
	sigaddset(&smask, SIGTERM);
	sigaddset(&smask, SIGINT);

	if (pthread_sigmask(SIG_BLOCK, &smask, NULL) < 0) {
		perror("pthread_sigmask()");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	if (pthread_create(&tid, NULL, thread_fn, NULL) < 0) {
		perror("pthread_create()");
		exit(EXIT_FAILURE);
	}

	pthread_mutex_lock(&cond_lock);
	while (flag == 0) {
		pthread_cond_wait(&cond, &cond_lock);
	}
	pthread_mutex_unlock(&cond_lock);

	printf("Time to quit, bye!\n");

	return 0;
}
