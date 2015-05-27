/* Write a program that accepts as arguments a list of files and / or directories and prints the
 * size of each file / directory. If an argument is a directory, the program recursively enters
 * the directory.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void dirwalk(const char *name, void (*f)(const char *));

void fsize(const char *name) {
	struct stat sbuff;

	if (stat(name, &sbuff) < 0) {
		fprintf(stderr, "can't access %s: ", name);
		perror(NULL);
		return;
	}

	if (S_ISDIR(sbuff.st_mode)) {
		dirwalk(name, fsize);
	}

	printf("%8ld %s\n", sbuff.st_size, name);
}

void dirwalk(const char *name, void (*f)(const char *)) {
	DIR *dir;

	dir = opendir(name);
	if (dir == NULL) {
		fprintf(stderr, "can't open directory %s: ", name);
		perror(NULL);
		return;
	}

	struct dirent *dir_entry;
	char path_buff[PATH_MAX];

	for (dir_entry = readdir(dir); dir_entry != NULL; dir_entry = readdir(dir)) {
		if (!strcmp(dir_entry->d_name, ".") ||
		    !strcmp(dir_entry->d_name, "..")) {
			continue;
		}
		if (strlen(name)+strlen(dir_entry->d_name)+2 > PATH_MAX) {
			fprintf(stderr, "Warning: path is too large; skipping: %s/%s\n",
				name, dir_entry->d_name);
			continue;
		}
		sprintf(path_buff, "%s/%s", name, dir_entry->d_name);
		(*f)(path_buff);
	}

	closedir(dir);
}

int main(int argc, char *argv[]) {
	size_t i;

	for (i = 1; i < argc; i++) {
		fsize(argv[i]);
	}

	return 0;
}
