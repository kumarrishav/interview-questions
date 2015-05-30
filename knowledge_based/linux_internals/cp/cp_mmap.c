
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define MAX_LEN (1024*1024*1024)

int main(int argc, char *argv[]) {
	int src, dst;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((src = open(argv[1], O_RDONLY)) < 0) {
		perror("Unable to open source file for reading");
		exit(EXIT_FAILURE);
	}

	if ((dst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
		perror("Unable to open target file for writing");
		exit(EXIT_FAILURE);
	}

	struct stat statbuf;
	if (fstat(src, &statbuf) < 0) {
		perror("Couldn't determine source file size");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(dst, statbuf.st_size) < 0) {
		perror("Couldn't set target file size");
		exit(EXIT_FAILURE);
	}

	off_t written = 0;

	while (written < statbuf.st_size) {
		off_t incr;
		if (statbuf.st_size-written > MAX_LEN) {
			incr = MAX_LEN;
		} else {
			incr = statbuf.st_size-written;
		}

		void *src_buf, *dst_buf;

		if ((src_buf = mmap(NULL, incr, PROT_READ, MAP_SHARED, src, written)) == MAP_FAILED) {
			perror("mmap() failed on source file");
			exit(EXIT_FAILURE);
		}

		if ((dst_buf = mmap(NULL, incr, PROT_WRITE, MAP_SHARED, dst, written)) == MAP_FAILED) {
			perror("mmap() failed on destination file");
			exit(EXIT_FAILURE);
		}

		memcpy(dst_buf, src_buf, incr);
		munmap(src_buf, incr);
		munmap(dst_buf, incr);

		written += incr;
	}

	return 0;
}
