/* Write a program that recursively finds every filename that ends in ".c" in the current directory.
 *
 * Source: Careercup (Amazon interview)
 */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#define PATTERN ".c"

const char *strrstr(const char *haystack, const char *needle) {
	if (haystack == NULL || needle == NULL) {
		return NULL;
	}

	size_t haystack_len = strlen(haystack);
	size_t needle_len = strlen(needle);

	if (needle_len > haystack_len) {
		return NULL;
	}

	ssize_t i = haystack_len-needle_len;
	while (i >= 0) {
		if (!strncmp(&haystack[i], needle, needle_len)) {
			return &haystack[i];
		}
		i--;
	}

	return NULL;
}

void find_c_files(const char *dir_name, size_t cwd_i) {
	static char path_buff[4096];
	size_t space_left = sizeof(path_buff)-cwd_i;
	int chars_added;

	chars_added = snprintf(&path_buff[cwd_i], space_left, "%s/", dir_name);
	if (chars_added >= space_left) {
		fprintf(stderr, "Warning: pathname too long, skipping (%s)\n", path_buff);
		return;
	}

	DIR *dir = opendir(path_buff);
	struct dirent *dent;

	while ((dent = readdir(dir)) != NULL) {
		if (dent->d_type == DT_DIR && strcmp(dent->d_name, ".") && strcmp(dent->d_name, "..")) {
			find_c_files(dent->d_name, cwd_i+chars_added);
			
		} else if (dent->d_type == DT_REG) {
			const char *ptr = strrstr(dent->d_name, PATTERN);
			if (ptr != NULL && !strcmp(ptr, PATTERN)) {
				printf("%s%s\n", path_buff, dent->d_name);
			}
		}
	}
}

int main(void) {
	find_c_files(".", 0);
	return 0;
}
