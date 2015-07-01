/* This program tests whether the operating system supports mandatory locking.
 * It works by having the parent process acquire a write lock in the entire file,
 * and then a child process attempts to read from the file without protection.
 *
 * Advanced Programming in the UNIX Environment claims that Solaris 10 supports mandatory
 * locking. Sadly, we can't say the same about Linux:
 *
 * filipe@filipe-Kubuntu:~$ ./a.out test 
 * Read lock denied, error = 11 (Resource temporarily unavailable)
 * read(2) returned success: abcdefg; mandatory locking doesn't work.
 * filipe@filipe-Kubuntu:~$
 *
 * Further investigation on the manpage of fcntl(2) reveals that the Linux implementation of
 * mandatory locks is unreliable and should not be used:
 *
 * The implementation of mandatory locking in all known versions of Linux is subject to race
 * conditions which render it unreliable: a write(2) call that overlaps with a lock may modify data
 * after the mandatory lock is acquired; a read(2) call that overlaps with a lock may detect changes
 * to data that were made only after a write lock was acquired. Similar races exist between
 * mandatory locks and mmap(2). It is therefore inadvisable to rely on mandatory locking.
 *
 * Another reason read(2) may have returned successfully in the above interaction is that the
 * filesystem where this was tested was not mounted with the `mand` option. According to the
 * manpage, the filesystem needs to be mounted with `-o mand` if using the mount(1) command,
 * or with the `MS_MANDLOCK` flag if using mount(2).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#define FILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* Parent <-> Child synchronization primitives
 * See knowledge_based/linux_internals/fork_sync for further info
 */

static volatile sig_atomic_t sig_flag;
static sigset_t old_nousr;
static struct sigaction sig_action_buf;

static void sigusr_handler(int signo) {
	sig_flag = 1;
}

void NOTIFY_INIT(void) {
	sig_flag = 0;

	sig_action_buf.sa_handler = sigusr_handler;
	sigemptyset(&sig_action_buf.sa_mask);
	sig_action_buf.sa_flags = 0;
	sigaction(SIGUSR1, &sig_action_buf, NULL);


	sigset_t usrmask;
	sigemptyset(&usrmask);
	sigaddset(&usrmask, SIGUSR1);

	// Aditionally block SIGUSR1
	sigprocmask(SIG_BLOCK, &usrmask, &old_nousr);
	sigdelset(&old_nousr, SIGUSR1);
}

void NOTIFY(pid_t pid) {
	kill(pid, SIGUSR1);
}

void WAIT_NOTIFY(void) {
	while (sig_flag == 0) {
		sigsuspend(&old_nousr);
	}

	sig_flag = 0;
}

int main(int argc, char *argv[]) {

	static const char file_contents[] = "abcdefg";
	static char readbuff[sizeof(file_contents)];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILEMODE);
	if (fd < 0) {
		fprintf(stderr, "Couldn't open %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Activate mandatory locking on the file */
	struct stat statbuf;
	if (fstat(fd, &statbuf) < 0) {
		fprintf(stderr, "stat(2) failed for file %s: %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
		perror("Couldn't enable mandatory locking");
		exit(EXIT_FAILURE);
	}

	if (write(fd, file_contents, sizeof(file_contents)-1) < 0) {
		perror("write(2) error");
		exit(EXIT_FAILURE);
	}

	NOTIFY_INIT();

	pid_t f;
	if ((f = fork()) < 0) {
		perror("fork(2) failed");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		/* Parent goes first */
		WAIT_NOTIFY();

		/* We need to change to nonblocking I/O to prevent read(2) from blocking waiting for
		 * the lock to be released
		 */
		if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
			perror("fcntl(2) failed");
			exit(EXIT_FAILURE);
		}

		/* Let's see what error we get when we attempt to read-lock a file that is already
		 * write-locked
		 */
		struct flock lock;
		lock.l_type = F_RDLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;

		if (fcntl(fd, F_SETLK, &lock) < 0) {
			printf("Read lock denied, error = %d (%s)\n", errno, strerror(errno));
		} else {
			fprintf(stderr, "Oops! Read lock acquired when parent process has write lock.. ?!\n");
			exit(EXIT_FAILURE);
		}

		/* Now let's try to read without holding a lock */
		if (lseek(fd, 0, SEEK_SET) < 0) {
			perror("lseek(2) failed");
			exit(EXIT_FAILURE);
		}

		ssize_t r;
		if ((r = read(fd, readbuff, sizeof(readbuff)-1)) < 0) {
			printf("read(2) returned with error %d (%s); mandatory locking seems to work.\n", errno, strerror(errno));
		} else {
			readbuff[r] = '\0';
			printf("read(2) returned success: %s; mandatory locking doesn't work.\n", readbuff);
		}
	} else {
		/* Acquire write lock for the entire file */
		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;

		if (fcntl(fd, F_SETLK, &lock) < 0) {
			perror("fcntl(2) failed");
			NOTIFY(f);
			exit(EXIT_FAILURE);
		}

		NOTIFY(f);
		wait(NULL);
	}

	return 0;
}
