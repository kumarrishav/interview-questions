
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

sem_t *semaphore;

int main(void) {
	semaphore = mmap(NULL, sizeof(*semaphore), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (semaphore == MAP_FAILED) {
		perror("mmap(2) error");
		exit(EXIT_FAILURE);
	}

	if (sem_init(semaphore, 1, 1) < 0) {
		perror("sem_init(3) error");
		exit(EXIT_FAILURE);
	}

	pid_t pid1, pid2;
	if ((pid1 = fork()) < 0) {
		perror("fork(2) error on first child");
	}
	if (pid1 != 0 && (pid2 = fork()) < 0) {
		perror("fork(2) error on second child");
		exit(EXIT_FAILURE);
	}

	if (pid1 != 0 && pid2 != 0) {
		if (waitpid(pid1, NULL, 0) < 0)
			perror("Error waiting for 1st child");
		if (waitpid(pid2, NULL, 0) < 0)
			perror("Error waiting for 2nd child");
		return 0;
	}

	while (1) {
		if (sem_wait(semaphore) < 0) {
			perror("sem_wait(3) error");
			exit(EXIT_FAILURE);
		}

		printf("PID %ld got semaphore\n", (long) getpid());

		if (sem_post(semaphore) < 0) {
			perror("sem_post(3) error");
		}

		sleep(3);
	}

	return 0;
}
