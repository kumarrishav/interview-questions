/* Write a utility like cp(1) that copies a file containing holes, without writing the bytes of 0 to
 * the output file.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define RWRWR (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

static char buf[4096];

int main(int argc, char *argv[]) {

	if (argc < 3) {
		fprintf(stderr, "Usage: %s source destination\n", argv[0]);
		return 128;
	}

	int src = open(argv[1], O_RDONLY);
	if (src < 0) {
		fprintf(stderr, "Couldn't open %s: ", argv[1]);
		perror(NULL);
		return 128;
	}

	int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, RWRWR);
	if (dest < 0) {
		fprintf(stderr, "Couldn't create %s: ", argv[2]);
		perror(NULL);
		return 128;
	}

	int n;
	while ((n = read(src, buf, sizeof(buf))) > 0) {
		int i = 0;
		while (i < n) {
			int begin = i;
			while (i < n && buf[i] == '\0') {
				i++;
			}
			if (lseek(dest, i-begin, SEEK_CUR) < 0) {
				perror("Seek error");
				return 128;
			}
			begin = i;
			while (i < n && buf[i] != '\0') {
				i++;
			}
			if (write(dest, buf+begin, i-begin) < i-begin) {
				perror("Write error");
				return 128;
			}
		}
	}

	if (n < 0) {
		perror("Read error");
		return 128;
	}

	return 0;
}
