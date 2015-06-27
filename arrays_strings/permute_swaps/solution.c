/* Given a source string and a target string that is a permutation of the source string, print the
 * steps necessary to go from source to target if only adjacent swaps are allowed.
 *
 * EXAMPLE
 *
 * Source: cat
 * Target: tac
 *
 * Possible output: cat -> act -> atc -> tac
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void permute_str_aux(char src[], char target[], size_t len) {
	if (len == 0) {
		return;
	}

	ssize_t i;
	for (i = len-1; i >= 0 && src[i] != target[len-1]; i--)
		; /* Intentionally left blank */

	assert(i >= 0);

	for (; i < len-1; i++) {
		char tmp = src[i+1];
		src[i+1] = src[i];
		src[i] = tmp;
		printf(" -> %s", src);
	}

	permute_str_aux(src, target, len-1);
}

void permute_str(char src[], char target[]) {
	size_t len = strlen(src);
	assert(len == strlen(target));

	printf("%s", src);
	permute_str_aux(src, target, len);
	printf("\n");
}

static char str1_buff[1024];
static char str2_buff[1024];

int main(void) {
	printf("Enter source and target strings\n");
	printf("> ");

	while (scanf("%s%s", str1_buff, str2_buff) == 2) {
		permute_str(str1_buff, str2_buff);
		printf("> ");
	}

	return 0;
}
