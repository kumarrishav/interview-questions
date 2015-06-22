/* This program illustrates the use of fork handlers in the pthread library.
 *
 * Fork handlers are important for multithreaded code that forks a new process.
 * Let's consider what happens when a multithreaded program forks:
 *
 * - Only the calling thread is duplicated in the new process
 * - The entire address space is copied, including every lock of every thread and their state
 *
 * This leads to a major problem: the child process finds itself in a situation where locks are
 * acquired by threads that do not exist.
 *
 * The child could simply reinitialize the locks, but this is not enough: other threads that do not
 * exist in the child process could be in the middle of a critical region and may have left an
 * operation incomplete, with inconsistent state. The child process does not own the locks and has
 * no way to recover from this unfortunate situation.
 *
 * Fork handlers give a chance to the process to do some cleanup and preparation before bringing a
 * child to life. There are 3 types of handlers:
 *
 * - prepare: this handler is called in the process before a child is created
 * - parent: this handler is called in the parent process after a child is created, but before
 *   fork(2) returns
 * - child: this handler is called in the child process before fork(2) returns
 *
 * The expected sequence of operations is as follows:
 *
 * 1. Process calls fork(2)
 * 2. The prepare handler is called, and the process is expected to acquire every lock in that
 *    handler. This ensures that no other threads hold any lock before forking. It also guarantees
 *    that the state is consistent (as long as threads are well-behaved, of course)
 * 3. A child is born
 * 4. The parent handler is called in the parent process, and the child handler is called in the
 *    child. It is expected that these handlers unlock the locks acquired by the prepare handler.
 *
 * Note that it may appear that step 4 unlocks the same lock twice; but remember that the new child
 * process contains a memory copy of the parent process. Each handler is unlocking the locks
 * acquired before the copy was done, but they do so in their separate private copy.
 *
 * It is possible to register multiple fork handlers by calling pthread_atfork() more than once.
 * In this case, the order in which handlers are called is different: prepare handlers are called
 * in the opposite order in which they were registered, whereas child and parent handlers are
 * called in the order in which they were registered. This behavior ensures that the sequence of
 * locks acquired in the prepare handler is the opposite of the sequence of locks released.
 *
 * So, for example, if Module A acquires a set of locks before calling Module B, which also has its
 * own locks, then we should register first the handlers for Module B, and then the handlers for
 * Module A. Upon forking, the preparation handler for A is called, where A acquires its locks,
 * and then the preparation handler for B is called. After forking, the child/parent handler for B
 * is called first, unlocking B's locks, and only then is A allowed to unlock.
 *
 * Sadly, fork handlers do not solve the problem completely:
 *
 * - It is impossible to clean the state of other complex synchronization primitives, such as
 *   condition variables and barriers. For example, if condition variables embed a mutex in its
 *   struct, then there is no way to recover. If, on the other hand, the library uses a global
 *   mutex to control access to every data structure, then the library may protect itself
 *   by registering its own fork handlers, but user code shouldn't rely on this, as it's an
 *   implementation detail. The sad truth is that there is no buller proof way to fork
 *   and gracefully reinitialize condition variables / mutexes.
 *
 * - If the code uses recursive mutexes, it is impossible to lock / unlock from within the handlers,
 *   because we don't know how many times we would have to unlock.
 *
 * - Some mutex implementations with additional error checking may return errors when the child
 *   attempts to unlock them.
 *
 * For these reasons, it is generally a good idea to avoid forking in multithreaded environments,
 * unless the child calls one of the seven exec() functions. POSIX states that child processes
 * can only call async-signal safe functions during the time window between the return from fork()
 * and the call to exec().
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

static unsigned long long counter;
static unsigned long long counter2;

void prepare_handler(void) {
	pthread_mutex_lock(&lock1);
	pthread_mutex_lock(&lock2);
	printf("Prepare: acquired locks\n");
}

void parent_handler(void) {
	pthread_mutex_unlock(&lock2);
	pthread_mutex_unlock(&lock1);
	printf("Parent: released locks\n");
	
}

void child_handler(void) {
	pthread_mutex_unlock(&lock2);
	pthread_mutex_unlock(&lock1);
	printf("Child: released locks\n");
}

void *thread_fn(void *arg) {
	printf("Thread started. Press return to continue.\n");
	while (1) {
		pthread_mutex_lock(&lock1);
		counter++;
		pthread_mutex_unlock(&lock1);
		pthread_mutex_lock(&lock2);
		counter2++;
		pthread_mutex_unlock(&lock2);
	}
}

int main(void) {
	if (pthread_atfork(prepare_handler, parent_handler, child_handler) < 0) {
		perror("pthread_atfork()");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	if (pthread_create(&tid, NULL, thread_fn, NULL) < 0) {
		perror("pthread_create()");
		exit(EXIT_FAILURE);
	}

	getchar();

	pid_t f;
	if ((f = fork()) < 0) {
		perror("fork()");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		printf("Child leaving\n");
		exit(0);
	} else {
		wait(NULL);
		printf("Parent leaving\n");
	}

	return 0;
}
