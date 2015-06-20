/* This file implements a multithreaded work queue system where a master thread inserts jobs into
 * the queue and worker threads pick them.
 *
 * The system works as follows:
 * - Worker threads sleep until the queue becomes non-empty. When that happens, at least one of
 *   the worker threads is woken up by the master thread using condition variables.
 *
 * - The master thread places a new job in the queue and uses condition variables to notify the
 *   worker threads that the state of the queue has changed.
 *
 * The use of condition variables here is important to avoid polling. Without condition variables,
 * worker threads would wastefully test the state of the queue repeatedly. Instead, worker threads
 * sleep until the state of the queue changes.
 *
 * Note that the master thread signalizes the condition after it releases the mutex that protects
 * the work queue. Thus, there is a time window between releasing the mutex and signaling the
 * condition where a worker thread that is not sleeping accesses the queue and invalidates the
 * condition. This is why worker threads check that the queue is not empty even after returning
 * from pthread_cond_wait(3). Recall that pthread_cond_wait(3) must be called with the mutex locked,
 * because that mutex is used internally to add the current thread to the list of threads waiting
 * for a condition. Then, the mutex is unlocked inside pthread_cond_wait(3) and the thread is put to
 * sleep until the condition is signaled. Upon waking up, pthread_cond_wait(3) returns again with
 * the mutex locked.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// Number of worker threads
#define WORKERS 4

static struct job_node *workq;
static pthread_mutex_t workq_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t workq_cond = PTHREAD_COND_INITIALIZER;

static const char *const jobs_descriptions[] = {
	"Example job 1",
	"Example job 2",
	"Example job 3",
	"Example job 4",
	"Example job 5"
};

static pthread_t workers[WORKERS];
static pthread_t boss;

static const size_t jobs_sz = sizeof(jobs_descriptions)/sizeof(jobs_descriptions[0]);

struct job_node {
	const char *job_description;
	/* ... */
	struct job_node *next;
};

struct job_node *get_job(void) {
	pthread_mutex_lock(&workq_lock);
	// The while is necessary to avoid race conditions
	while (workq == NULL) {
		pthread_cond_wait(&workq_cond, &workq_lock);
	}
	struct job_node *ret = workq;
	workq = workq->next;
	pthread_mutex_unlock(&workq_lock);
	return ret;
}

void insert_job(struct job_node *new_job) {
	pthread_mutex_lock(&workq_lock);
	new_job->next = workq;
	workq = new_job;
	pthread_mutex_unlock(&workq_lock);
	pthread_cond_signal(&workq_cond);
}

void *worker_thr(void *arg) {
	while (1) {
		struct job_node *work = get_job();
		/* Warning: printing a thread ID is non-portable.
		 * Apparently, Linux defines pthread_t to be a pointer type to a struct
		 */
		printf("[%p]: %s\n", (void *) pthread_self(), work->job_description);
		free(work);
	}
	return NULL;
}

void *boss_thr(void *arg) {
	static size_t job_id;

	while (1) {
		struct job_node *newjob = malloc(sizeof(*newjob));
		if (newjob != NULL) {
			newjob->job_description = jobs_descriptions[job_id];
			job_id = (job_id+1)%jobs_sz;
			insert_job(newjob);
		}
	}

}

int main(void) {
	size_t i;
	for (i = 0; i < WORKERS; i++) {
		if (pthread_create(&workers[i], NULL, worker_thr, NULL) < 0) {
			perror("Couldn't create worker thread");
			exit(EXIT_FAILURE);
		}
	}

	if (pthread_create(&boss, NULL, boss_thr, NULL) < 0) {
		perror("Couldn't create boss thread");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < WORKERS; i++) {
		if (pthread_join(workers[i], NULL) < 0) {
			perror("Couldn't join thread");
			exit(EXIT_FAILURE);
		}
	}

	if (pthread_join(boss, NULL) < 0) {
		perror("Couldn't join with boss thread");
		exit(EXIT_FAILURE);
	}

	return 0;
}
