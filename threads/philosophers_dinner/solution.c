/* In the famous dining philosophers problem, a bunch of philosophers are sitting
 * around a circular table with one fork between each of them. A philosopher
 * needs both forks to eat.
 * Assume that 5 philosophers are sitting around the circular table.
 * Design and implement an algorithm that is free of deadlocks and ensures that every philosopher
 * will eventually eat.
 */
#include <pthread.h>
#include <stdio.h>
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

static pthread_mutex_t forks[] = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
				   PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
				   PTHREAD_MUTEX_INITIALIZER };
static size_t forks_sz = sizeof(forks)/sizeof(forks[0]);

static size_t philosophers[] = { 0, 1, 2, 3, 4 };
static size_t philosophers_sz = sizeof(philosophers)/sizeof(philosophers[0]);

static pthread_t philosophers_threads[sizeof(philosophers)/sizeof(philosophers[0])];

void *philosopher_thread(void *n) {
	size_t i = *(size_t *)n;

	size_t right_fork = i;
	size_t left_fork = (i+1)%forks_sz;

	size_t first_fork = min(right_fork, left_fork);
	size_t second_fork = max(right_fork, left_fork);

	pthread_mutex_lock(&forks[first_fork]);
	printf("[Philosopher %zu]: Picked up fork %zu\n", i, first_fork);

	pthread_mutex_lock(&forks[second_fork]);
	printf("[Philosopher %zu]: Picked up fork %zu\n", i, second_fork);

	printf("[Philosopher %zu]: Eating... Done!\n", i);

	pthread_mutex_unlock(&forks[second_fork]);
	pthread_mutex_unlock(&forks[first_fork]);

	return NULL;
}

int main(void) {

	size_t i;
	for (i = 0; i < philosophers_sz; i++) {
		pthread_create(&philosophers_threads[i], NULL, philosopher_thread, &philosophers[i]);
	}

	for (i = 0; i < philosophers_sz; i++) {
		pthread_join(philosophers_threads[i], NULL);
	}

	return 0;
}
