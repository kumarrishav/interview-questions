/* A simple parent / child synchronization mechanism using pipes */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int ptochld[2], ptoparent[2];

int TELL_WAIT(void) {
	if (pipe(ptochld) < 0) {
		return -1;
	}

	if (pipe(ptoparent) < 0) {
		close(ptochld[0]);
		close(ptochld[1]);
		return -1;
	}

	return 0;
}

void TELL_PARENT(void) {
	write(ptoparent[1], "p", 1);
}

void TELL_CHILD(void) {
	write(ptochld[1], "c", 1);
}

void WAIT_PARENT(void) {
	char c;
	read(ptochld[0], &c, 1);
}

void WAIT_CHILD(void) {
	char p;
	read(ptoparent[0], &p, 1);
}

static char msg_buff[] = "A message from a process.\n";

static void write_msgs(const char *proc) {
	int i;
	for (i = 0; i < 10; i++) {
		write(STDOUT_FILENO, proc, strlen(proc));
		write(STDOUT_FILENO, msg_buff, sizeof(msg_buff)-1);
		sleep(1);
	}
}

int main(void) {
	printf("Started\n");
	TELL_WAIT();

	pid_t f = fork();
	if (f < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		write_msgs("child: ");
		TELL_PARENT();
		WAIT_PARENT();
		printf("Child exiting\n");
		TELL_PARENT();
	} else {
		/* Child goes first */
		WAIT_CHILD();
		write_msgs("parent: ");
		printf("Press any key to terminate\n");
		getchar();
		TELL_CHILD();
		WAIT_CHILD();
		printf("Parent exiting\n");
	}

	return 0;
}
