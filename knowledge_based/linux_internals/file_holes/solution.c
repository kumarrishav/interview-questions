/* Explain what it means when a file has holes.
 * Write a program that creates a file with holes.
 */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static const char str1[] = "abcdefg";
static const size_t str1_sz = sizeof(str1)-1;

static const char str2[] = "ABCDEFG";
static const size_t str2_sz = sizeof(str2)-1;

int main(void) {
	int fd = creat("file.test", FILE_MODE);
	if (fd < 0) {
		perror("Unable to create new file");
		exit(127);
	}

	if (write(fd, str1, str1_sz) != str1_sz) {
		perror("Unable to write to file");
		close(fd);
		exit(128);
	}
	/* Offset is now str1_sz (7) */

	if (lseek(fd, 16384, SEEK_SET) < 0) {
		perror("File is not seekable");
		close(fd);
		exit(129);
	}

	if (write(fd, str2, str2_sz) != str2_sz) {
		perror("Unable to write to file");
		close(fd);
		exit(127);
	}

	close(fd);

	return 0;
}
