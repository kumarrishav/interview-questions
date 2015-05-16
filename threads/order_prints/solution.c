/* There are 3 different threads in a process.
 * The first thread prints '1' forever, the second thread prints '2' forever, and the
 * third thread prints '3' forever.
 * How would you force the output sequence to be '1' '2' '3' '1' '2' '3' ...?
 *
 * Source: Careercup (Salesforce interview)
 */
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

static sem_t semaphores[3];
static const size_t sem_sz = sizeof(semaphores)/sizeof(semaphores[0]);

static void *thread1(void *);
static void *thread2(void *);
static void *thread3(void *);

static pthread_t threads[3];
void *(*threads_fns[3])(void *) = { thread1, thread2, thread3 };
static const size_t threads_sz = sizeof(threads)/sizeof(threads[0]);

static void *thread1(void *arg) {
	while (1) {
		sem_wait(&semaphores[0]);
		putchar('1');
		putchar('\n');
		sem_post(&semaphores[1]);
	}
	return NULL;
}

static void *thread2(void *arg) {
	while (1) {
		sem_wait(&semaphores[1]);
		putchar('2');
		putchar('\n');
		sem_post(&semaphores[2]);
	}
	return NULL;
}

static void *thread3(void *arg) {
	while (1) {
		sem_wait(&semaphores[2]);
		putchar('3');
		putchar('\n');
		sem_post(&semaphores[0]);
	}
	return NULL;
}

int main(void) {
	size_t i;
	for (i = 0; i < sem_sz && sem_init(&semaphores[i], 0, 1) == 0; i++)
		; /* Intentionally left blank */

	if (i < sem_sz) {
		perror("Couldn't initialize semaphore");
		size_t j;
		for (j = 0; j < i; j++) {
			sem_destroy(&semaphores[j]);
		}
		return 128;
	}

	sem_wait(&semaphores[1]);
	sem_wait(&semaphores[2]);

	for (i = 0; i < threads_sz && pthread_create(&threads[i], NULL, threads_fns[i], NULL) == 0; i++)
		; /* Intentionally left blank */

	if (i < threads_sz) {
		perror("Couldn't create thread");
		size_t j;
		for (j = 0; j < i; j++) {
			pthread_cancel(threads[j]);
		}
		return 129;
	}

	for (i = 0; i < threads_sz; i++) {
		pthread_join(threads[i], NULL);
	}

	for (i = 0; i < sem_sz; i++) {
		sem_destroy(&semaphores[i]);
	}

	return 0;
}
