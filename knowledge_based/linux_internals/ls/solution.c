/* Write a bare-bones implementation of the linux ls command.
 * Your implementation should always list the current working directory.
 * If -R is used, the current working directory is listed recursively.
 */
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

void list_dir_aux(char cwd[PATH_MAX], size_t cwd_i, int recurse) {

	DIR *directory = opendir(cwd);
	if (directory == NULL) {
		fprintf(stderr, "Could not open directory %s: ", cwd);
		perror(NULL);
		return;
	}

	printf("%s:\n", cwd);

	errno = 0;
	struct dirent *de = readdir(directory);

	while (de != NULL) {
		printf("%s\n", de->d_name);

		size_t cwd_i_incr = sprintf(&cwd[cwd_i], "%s", de->d_name);
		struct stat s;

		if (stat(cwd, &s) != 0) {
			fprintf(stderr, "Could not get into about %s: ", cwd);
			perror(NULL);
		} else {
			if (recurse && S_ISDIR(s.st_mode) &&
			    strcmp(".", de->d_name) && strcmp("..", de->d_name)) {
				cwd[cwd_i+cwd_i_incr] = '/';
				cwd_i_incr++;
				cwd[cwd_i+cwd_i_incr] = '\0';
				list_dir_aux(cwd, cwd_i+cwd_i_incr, recurse);
			}
		}

		errno = 0;
		de = readdir(directory);
	}

	if (errno != 0) {
		perror("Error reading directory entry");
	}

	closedir(directory);
}

void list_curr_dir(int recurse) {
	static char cwd[PATH_MAX];
	sprintf(cwd, "./");
	list_dir_aux(cwd, 2, recurse);
}

int main(int argc, char *argv[]) {
	int recurse;

	if (argc < 2) {
		recurse = 0;
	} else {
		if (strcmp(argv[1], "-R")) {
			fprintf(stderr, "Unrecognized option: %s\n", argv[1]);
			return 127;
		}
		recurse = 1;
	}

	list_curr_dir(recurse);

	return 0;
}
