/* What synchronization primitives would you need to implement a barrier?
 * Provide an implementation of the pthread_barrier_wait(3) function.
 *
 * Source: Advanced Programming in the UNIX Environment, 3rd Ed., Chapter 11, Exercise 11.5
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct thread_barrier {
	unsigned long long target;
	unsigned long long counter;
	pthread_cond_t condv;
	pthread_mutex_t lock;
};

int thread_barrier_init(struct thread_barrier *barrier, unsigned count) {
	if (pthread_cond_init(&barrier->condv, NULL) < 0) {
		return -1;
	}
	if (pthread_mutex_init(&barrier->lock, NULL) < 0) {
		pthread_cond_destroy(&barrier->condv);
		return -1;
	}

	barrier->target = count;
	barrier->counter = 0;

	return 0;
}

void thread_barrier_destroy(struct thread_barrier *barrier) {
	pthread_mutex_destroy(&barrier->lock);
	pthread_cond_destroy(&barrier->condv);
}

/* It is important to mention here that in this scenario, unlike in other examples,
 * we most definitely can't release the lock before calling pthread_cond_broadcast(3).
 * Unlocking the mutex only after broadcast is called guarantees that no additional threads
 * will block on pthread_cond_wait(3) and be mistakenly woken up by the last thread of the previous
 * barrier use.
 * An example often helps. Assume that we unlock the mutex and then call broadcast. Further assume
 * that the barrier counter has been initialized to 5. Here's what could happen:
 * - Thread1, thread2, thread3 and thread4 call thread_barrier_wait() and block in
 *   pthread_cond_wait().
 * - Thread5 enters barrier_wait and notices it's the last thread on the barrier. It is ready to
 *   wake up every thread that is waiting on the barrier.
 * - Thread5 unlocks the mutex, but it loses CPU just before calling broadcast on the condition
 *   variable.
 * - Meanwhile, another set of new threads, say, thread6, thread7 and thread8, all called
 *   thread_barrier_wait() and blocked in pthread_cond_wait().
 * - Thread5 is rescheduled and resumes execution, calling broadcast().
 * - Now, all 8 threads are wrongfully woken up. A barrier that was supposed to synchronize
 *   5 threads ended up synchronizing 8 threads. This breaks the specification.
 *
 * To avoid this, the thread that broadcasts cannot unlock before calling broadcast.
 * By calling broadcast first, we are sure that any possible new threads are blocked trying to
 * acquire the mutex (rather than being blocked on cond_wait()), and so the broadcast will
 * wake up the correct threads only.
 */
int thread_barrier_wait(struct thread_barrier *barrier) {
	pthread_mutex_lock(&barrier->lock);
	if (++barrier->counter == barrier->target) {
		barrier->counter = 0;
		pthread_cond_broadcast(&barrier->condv);
		pthread_mutex_unlock(&barrier->lock);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	} else {
		pthread_cond_wait(&barrier->condv, &barrier->lock);
		pthread_mutex_unlock(&barrier->lock);
		return 0;
	}
}

/* A sample toy program where every thread increments a counter up to a given threshold,
 * synchronizing with other threads each `step` increments
 */

static const unsigned long long target_value = 8000000;
static unsigned long long counter = 0;
pthread_mutex_t counter_lock = PTHREAD_MUTEX_INITIALIZER;
/* Each thread synchronizes with the others every 250000 increments */
static const unsigned long long sync_step = 250000;
/* Each thread does a total of 1000000 increments */
static const unsigned long long thread_inc = 1000000;

static struct thread_barrier barrier;

void *thr_fn(void *arg) {
	unsigned long long i;
	for (i = 1; i <= thread_inc; i++) {
		pthread_mutex_lock(&counter_lock);
		counter++;
		pthread_mutex_unlock(&counter_lock);
		if (i%sync_step == 0) {
			/* Time to sync */
			printf("[%p] Waiting...\n", (void *) pthread_self());
			if (thread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD) {
				printf("[%p] Last on the barrier\n", (void *) pthread_self());
			} else {
				printf("[%p] Woke up\n", (void *) pthread_self());
			}
		}
	}
	return NULL;
}

int main(void) {
	const unsigned long long threads_no = target_value/thread_inc;

	if (thread_barrier_init(&barrier, threads_no) < 0) {
		perror("thread_barrier_init()");
		exit(EXIT_FAILURE);
	}

	pthread_t threads[threads_no];

	size_t i;
	for (i = 0; i < threads_no; i++) {
		if (pthread_create(&threads[i], NULL, thr_fn, NULL) < 0) {
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < threads_no; i++) {
		if (pthread_join(threads[i], NULL) < 0) {
			perror("pthread_join()");
			exit(EXIT_FAILURE);
		}
	}

	printf("All done, counter = %lld\n", counter);

	return 0;
}
