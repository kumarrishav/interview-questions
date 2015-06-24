
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>

#define MAX_STR_SIZE 4096

extern char **environ;

pthread_key_t env_key;
pthread_mutex_t key_lock = PTHREAD_MUTEX_INITIALIZER;

char *getenv_thr_safe(const char *name) {

	char *envbuf = pthread_getspecific(env_key);

	if (envbuf == NULL) {
		envbuf = malloc(MAX_STR_SIZE);
		if (envbuf == NULL) {
			return NULL;
		}
		pthread_setspecific(env_key, envbuf);
	}

	size_t name_sz = strlen(name);
	char *ret = NULL;

	pthread_mutex_lock(&key_lock);
	size_t i;
	for (i = 0; environ[i] != NULL; i++) {
		if (strncmp(environ[i], name, name_sz) == 0 &&
		    environ[i][name_sz] == '=') {
			strncpy(envbuf, &environ[i][name_sz+1], MAX_STR_SIZE-1);
			envbuf[MAX_STR_SIZE-1] = '\0';
			ret = envbuf;
			break;
		}
	}
	pthread_mutex_unlock(&key_lock);

	return ret;
}

void *thr_fn(void *arg) {
	char *var = getenv_thr_safe(arg);
	printf("[%p]: %s = %s\n", (void *) pthread_self(), (char *) arg, var);
	return NULL;
}

#define THREADS_CREATED 4

int main(void) {

	if (pthread_key_create(&env_key, free) < 0) {
		perror("pthread_key_create()");
		exit(EXIT_FAILURE);
	}

	pthread_t threads[THREADS_CREATED];
	char *names[] = { "PATH", "SHELL", "PWD", "HOME" };
	size_t i;

	for (i = 0; i < THREADS_CREATED; i++) {
		if (pthread_create(&threads[i], NULL, thr_fn, names[i]) < 0) {
			perror("pthread_create()");
			exit(EXIT_FAILURE);
		}
	}

	for (i = 0; i < THREADS_CREATED; i++) {
		if (pthread_join(threads[i], NULL) < 0) {
			perror("pthread_join()");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
