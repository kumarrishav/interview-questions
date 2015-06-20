/* This is a multithreaded worker queue implemented with semaphores.
 *
 * This approach is in some ways more elegant than the implementation that uses condition variables:
 *
 * - get_job() doesn't need to reevaluate to make sure it still holds. Semaphores keep the counter
 *   state internally, so there's no harm if the master thread calls sem_post() when none of the
 *   worker threads is blocked in sem_wait() - they will eventually wait on the semaphore, and when
 *   that happens, sem_wait() returns immediately, as opposed to pthread_cond_wait(), which blocks
 *   when the master issues pthread_cond_signal() before the worker calls pthread_cond_wait().
 *   Also, since a semaphore is just a thread-safe counter, it is not possible for sem_wait() to
 *   return and the queue be empty, because the master thread posts to the semaphore for each item
 *   inserted   
 */

#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define WORKERS 4

static struct job_node *workq;
static sem_t workq_sem;
static pthread_mutex_t workq_lock = PTHREAD_MUTEX_INITIALIZER;

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
	sem_wait(&workq_sem);
	pthread_mutex_lock(&workq_lock);
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
	sem_post(&workq_sem);
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

	if (sem_init(&workq_sem, 0, 0) < 0) {
		perror("Couldn't initialize semaphore");
		exit(EXIT_FAILURE);
	}

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

	sem_destroy(&workq_sem);

	return 0;
}
