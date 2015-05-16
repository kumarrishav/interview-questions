/* Write a program that identifies the type of each file passed as an argument */
#include <stdio.h>
#include <sys/stat.h>

const char *file_type(const char *path) {
	struct stat s;

	/* We use lstat because we don't want to follow symlinks */
	if (lstat(path, &s) < 0) {
		fprintf(stderr, "Unable to get info for %s: ", path);
		perror(NULL);
		return NULL;
	}

	if (S_ISREG(s.st_mode)) {
		return "regular file";
	} else if (S_ISDIR(s.st_mode)) {
		return "directory";
	} else if (S_ISCHR(s.st_mode)) {
		return "character device";
	} else if (S_ISBLK(s.st_mode)) {
		return "block device";
	} else if (S_ISSOCK(s.st_mode)) {
		return "socket";
	} else if (S_ISFIFO(s.st_mode)) {
		return "fifo (named pipe)";
	} else if (S_ISLNK(s.st_mode)) {
		return "symbolic link";
	} else {
		return "unknown file type";
	}
}

int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		const char *type = file_type(argv[i]);
		if (type != NULL) {
			printf("%s: %s\n", argv[i], type);
		}
	}
	return 0;
}
