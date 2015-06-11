/* Write a program that truncates every file passed as an argument but keeps its original
 * access and modification times
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/* This example shows the results for two files, `xpto` and `example`,
 * where the original values of atime, mtime and ctime were 06/11 19:55
 *
 * filipe@filipe-Kubuntu:~$ ls -l xpto example # mtime
 * -rw-rw-r-- 1 filipe filipe 58 Jun 11 19:55 example
 * -rw-rw-r-- 1 filipe filipe 21 Jun 11 19:55 xpto
 *
 * filipe@filipe-Kubuntu:~$ ls -lu xpto example # atime
 * -rw-rw-r-- 1 filipe filipe 58 Jun 11 19:55 example
 * -rw-rw-r-- 1 filipe filipe 21 Jun 11 19:55 xpto
 * filipe@filipe-Kubuntu:~$ ls -lc xpto example # ctime
 * -rw-rw-r-- 1 filipe filipe 58 Jun 11 19:55 example
 * -rw-rw-r-- 1 filipe filipe 21 Jun 11 19:55 xpto
 *
 * After running this program, both mtime and atime remain untouched because of the call
 * to futimens(), but ctime is changed:
 *
 * filipe@filipe-Kubuntu:~$ date
 * Qui Jun 11 19:56:20 WEST 2015
 * filipe@filipe-Kubuntu:~$ ./a.out xpto example
 * filipe@filipe-Kubuntu:~$ ls -l xpto example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:55 example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:55 xpto
 * filipe@filipe-Kubuntu:~$ ls -lu xpto example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:55 example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:55 xpto
 * filipe@filipe-Kubuntu:~$ ls -lc xpto example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:56 example
 * -rw-rw-r-- 1 filipe filipe 0 Jun 11 19:56 xpto
 *
 */

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file1 file2 file3 ... fileN\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int i;
	for (i = 1; i < argc; i++) {

		struct stat statbuf;
		if (stat(argv[i], &statbuf) < 0) {
			fprintf(stderr, "Error collecting file data for %s: %s\n", argv[i], strerror(errno));
			continue;
		}

		int fd;
		if ((fd = open(argv[i], O_RDWR | O_CREAT | O_TRUNC)) < 0) {
			fprintf(stderr, "Couldn't open %s: %s\n", argv[i], strerror(errno));
			continue;
		}

		struct timespec times[2];
		times[0] = statbuf.st_atim;
		times[1] = statbuf.st_mtim;

		if (futimens(fd, times) < 0) {
			fprintf(stderr, "Error changing file access and modification times for %s: %s\n", argv[i], strerror(errno));
		}

		close(fd);
	}

	return 0;
}
