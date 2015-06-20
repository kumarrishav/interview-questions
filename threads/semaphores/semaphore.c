/* Implement a semaphore library. How can you avoid busy waiting on sem_wait()? */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct semaphore {
	pthread_cond_t sem_cond;
	pthread_mutex_t sem_lock;
	unsigned long long counter;
};

int semaphore_init(struct semaphore *sem, unsigned int value) {
	if (pthread_cond_init(&sem->sem_cond, NULL) < 0) {
		return -1;
	}
	if (pthread_mutex_init(&sem->sem_lock, NULL) < 0) {
		pthread_cond_destroy(&sem->sem_cond);
		return -1;
	}
	sem->counter = value;
	return 0;
}

void semaphore_destroy(struct semaphore *sem) {
	pthread_cond_destroy(&sem->sem_cond);
	pthread_mutex_destroy(&sem->sem_lock);
}

void semaphore_wait(struct semaphore *sem) {
	pthread_mutex_lock(&sem->sem_lock);
	while (sem->counter == 0) {
		pthread_cond_wait(&sem->sem_cond, &sem->sem_lock);
	}
	sem->counter--;
	pthread_mutex_unlock(&sem->sem_lock);
}

void semaphore_post(struct semaphore *sem) {
	pthread_mutex_lock(&sem->sem_lock);
	sem->counter++;
	pthread_mutex_unlock(&sem->sem_lock);
	pthread_cond_signal(&sem->sem_cond);
}



static struct semaphore sem;

void *thr_fn(void *arg) {
	while (1) {
		semaphore_wait(&sem);
		printf("[%p]: Woke up\n", (void *) pthread_self());
	}
	return NULL;
}

static pthread_t threads[1024];
static size_t threads_sz;

int main(void) {

	if (semaphore_init(&sem, 0) < 0) {
		perror("Couldn't create semaphore");
		exit(EXIT_FAILURE);
	}

	printf("To post the semaphore N times, type: P N\n");
	printf("To quit, type: Q\n");
	printf("How many threads? ");
	scanf("%zu", &threads_sz);

	printf("Creating %zu threads\n", threads_sz);

	size_t i;
	for (i = 0; i < threads_sz; i++) {
		if (pthread_create(&threads[i], NULL, thr_fn, NULL) < 0) {
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}
		printf("Thread created: %p\n", (void *) threads[i]);
	}

	printf("> ");
	char op;
	while (scanf(" %c", &op) == 1) {
		if (op == 'P') {
			unsigned n;
			scanf("%u", &n);
			unsigned i;
			for (i = 0; i < n; i++) {
				semaphore_post(&sem);
			}
		} else if (op == 'Q') {
			break;
		} else {
			fprintf(stderr, "Unrecognized command: %c\n", op);
		}
		printf("> ");
	}

	for (i = 0; i < threads_sz; i++) {
		if (pthread_join(threads[i], NULL) < 0) {
			perror("pthread_join()");
			exit(EXIT_FAILURE);
		}
	}

	semaphore_destroy(&sem);

	return 0;
}
