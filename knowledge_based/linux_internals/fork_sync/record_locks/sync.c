/* A parent / child synchronizer using record locks.
 * Because locks are not inherited upon forking, the only possibility is to make
 * polling inside WAIT_NOTIFY()
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len) {
	struct flock lock;

	lock.l_start = offset;
	lock.l_len = len;
	lock.l_whence = whence;
	lock.l_type = type;

	return fcntl(fd, cmd, &lock);
}

#define read_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
	lock_reg(fd, F_SETLK, F_UNLCK, (offset), (whence), (len))

static char tmpfile_name[] = "_fork_sync_XXXXXX";
static int tmpfile_fd;

int NOTIFY_INIT(void) {
	tmpfile_fd = mkstemp(tmpfile_name);

	if (tmpfile_fd < 0) {
		return -1;
	}

	unlink(tmpfile_name);

	return 0;
}

void WAIT_NOTIFY(void) {
	static char pid_buf[32];
	int done = 0;
	while (!done) {
		readw_lock(tmpfile_fd, 0, SEEK_SET, 0);
		int n = pread(tmpfile_fd, pid_buf, sizeof(pid_buf)-1, 0);
		un_lock(tmpfile_fd, 0, SEEK_SET, 0);
		pid_buf[n] = '\0';
		long target_pid;
		done = (sscanf(pid_buf, "%ld", &target_pid) == 1 &&
			target_pid == (long) getpid());
	}
}

void NOTIFY(pid_t pid) {
	static char pid_buf[32];

	int len = sprintf(pid_buf, "%ld", (long) pid);

	writew_lock(tmpfile_fd, 0, SEEK_SET, 0);
	ftruncate(tmpfile_fd, 0);
	pwrite(tmpfile_fd, pid_buf, len, 0);
	un_lock(tmpfile_fd, 0, SEEK_SET, 0);
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
	NOTIFY_INIT();

	pid_t f = fork();
	if (f < 0) {
		perror("Couldn't fork");
		exit(EXIT_FAILURE);
	}

	if (f == 0) {
		write_msgs("child: ");
		NOTIFY(getppid());
		WAIT_NOTIFY();
		printf("Child exiting\n");
		NOTIFY(getppid());
	} else {
		/* Child goes first */
		WAIT_NOTIFY();
		write_msgs("parent: ");
		printf("Press any key to terminate\n");
		getchar();
		NOTIFY(f);
		WAIT_NOTIFY();
		printf("Parent exiting\n");
	}

	return 0;
}
