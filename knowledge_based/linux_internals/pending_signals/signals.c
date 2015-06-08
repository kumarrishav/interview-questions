/* Describe and show how a pending signal can be canceled.
 *
 * Source: StackOverflow
 */

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {

	sigset_t block_sigint, prev_mask;
	sigemptyset(&block_sigint);
	sigaddset(&block_sigint, SIGINT);

	if (sigprocmask(SIG_SETMASK, &block_sigint, &prev_mask) < 0) {
		perror("Couldn't block SIGINT");
		return 0;
	}

	printf("SIGINT blocked: kill -SIGINT %ld to generate a pending SIGINT. Press return when done.\n", (long) getpid());

	/* Now, open a new terminal and send SIGINT to this process.
	 *
	 * After doing that, the signal will be pending delivery because it is currently blocked.
	 *
	 * Now, if we ignore SIGINT, the pending signal will be cancelled
	 */

	getchar();

	struct sigaction ign_sigint, prev;
	ign_sigint.sa_handler = SIG_IGN;
	ign_sigint.sa_flags = 0;
	sigemptyset(&ign_sigint.sa_mask);

	if (sigaction(SIGINT, &ign_sigint, &prev) < 0) {
		perror("Couldn't ignore SIGINT");
		return 0;
	}

	printf("SIGINT ignored - pending SIGINT was canceled.\n");

	/* Now, restore the default action for SIGINT */
	if (sigaction(SIGINT, &prev, NULL) < 0) {
		perror("Couldn't restore default SIGINT behavior");
		return 0;
	}

	/* And also restore the process's original sigmask, which does not block SIGINT */
	if (sigprocmask(SIG_SETMASK, &prev_mask, NULL) < 0) {
		perror("Couldn't restore original process sigmask");
		return 0;
	}

	printf("Process sigmask and action for SIGINT are now restored\n");

	/* We will not receive SIGINT at this point because it was canceled
	 * So the process will block on getchar() here instead of terminating
	 */
	getchar();

	return 0;   
}
