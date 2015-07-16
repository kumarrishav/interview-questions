/* In the UNIX world, pathnames can contain the special entries "." and "..", which stand for
 * "current directory" and "parent directory".
 *
 * For example, the paths /home/google/example.txt and /home/google/./example.txt refer to the same
 * file.
 *
 * Similarly, the paths /home/google/foo/../example.txt and /home/google/foo/example.txt also refer
 * to the same file.
 *
 * Write a function that receives a path name and normalizes it by removing every "." and ".."
 * entries in the path.
 *
 * Your function should identify error conditions and stop processing as soon as that happens,
 * leaving the input string intact.
 *
 * EXAMPLES
 *
 * Input:  /a/b/../foo.txt
 * Output: /a/foo.txt
 *
 * Input:  /a/b/././../foo.txt
 * Output: /a/foo.txt
 *
 * Source: Careercup (Google interview)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct stack_node {
	size_t begin;
	size_t len;
};

static size_t next_token(char path[], size_t curr_i) {
	size_t i = curr_i;
	while (path[i] != '\0' && path[i] != '/')
		i++;
	return path[i] == '\0' ? i : i+1;
}

int normalize_path(char path[]) {
	if (path == NULL || path[0] == '\0') {
		return 0;
	}

	size_t path_len = strlen(path);
	size_t stack_sz = (path_len-1)/2+1;
	struct stack_node *stack;
	size_t st_top = 0;

	if ((stack = malloc(sizeof(*stack)*stack_sz)) == NULL) {
		errno = ENOMEM;
		return -1;
	}

	size_t i = 0;
	while (path[i] != '\0') {
		size_t j = next_token(path, i);
		char tmp = path[j];
		path[j] = '\0';

		if (!strcmp(&path[i], "../") || !strcmp(&path[i], "..")) {
			if (st_top == 0) {
				errno = EINVAL;
				path[j] = tmp;
				free(stack);
				return -1;
			}
			st_top--;
		} else if (strcmp(&path[i], "./") && strcmp(&path[i], ".")) {
			stack[st_top].begin = i;
			stack[st_top].len = j-i;
			st_top++;
		}

		path[j] = tmp;
		i = j;
	}

	size_t j = 0;
	for (i = 0; i < st_top; i++) {
		strncpy(&path[j], &path[stack[i].begin], stack[i].len);
		j += stack[i].len;
	}

	path[j] = '\0';
	free(stack);

	return 0;
}

static char path_buff[1024];

int main(void) {
	printf("Enter a path to normalize\n");
	printf("> ");

	while (scanf("%s", path_buff) == 1) {
		printf("Entered:\t%s\n", path_buff);
		if (normalize_path(path_buff) < 0) {
			perror("Error normalizing path");
		} else {
			printf("Normalized:\t%s\n", path_buff);
		}
		printf("> ");
	}

	return 0;
}
