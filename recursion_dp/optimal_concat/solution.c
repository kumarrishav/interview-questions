/* There's a function that concatenates two strings and returns the length of the resulting string.
 * We want to minimize the total concatenation cost of a series of strings.
 *
 * Say we have 3 strings: "abc", "def" and "gh". We would merge "abc" and "def" first, which
 * costs 6, and then merge "abcdef" with "gh", costing 8, and the total cost would be 14.
 *
 * However, if we merge "abc" and "gh" first, which costs 5, and then merge "abcgh" with "def",
 * which costs 8, the total cost would be 13.
 *
 * Write an algorithm to minimize the cost of concatenating a series of strings.
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void swap(char *arr[], size_t i, size_t j) {
	char *tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

unsigned optimal_merge_aux(size_t curr_cost, size_t curr_len, char *strs[], size_t curr_i, size_t strs_sz) {
	if (curr_i == strs_sz) {
		return curr_cost;
	}

	size_t i;
	size_t res = -1;

	for (i = curr_i; i < strs_sz; i++) {
		swap(strs, i, curr_i);

		size_t cost_tmp;
		size_t cost_i = strlen(strs[curr_i])+curr_len;

		cost_tmp = optimal_merge_aux(curr_cost+cost_i, cost_i, strs, curr_i+1, strs_sz);

		if (cost_tmp < res) {
			res = cost_tmp;
		}

		swap(strs, i, curr_i);
	}

	return res;
}

unsigned optimal_merge(char *strs[], size_t strs_sz) {
	unsigned res = -1;

	size_t i;
	for (i = 0; i < strs_sz; i++) {
		swap(strs, 0, i);
		unsigned res_tmp;
		res_tmp = optimal_merge_aux(0, strlen(strs[0]), strs, 1, strs_sz);
		if (res_tmp < res) {
			res = res_tmp;
		}
		swap(strs, 0, i);
	}

	return res;
}

static char *strings[1024];
static char buff[1024];

int main(void) {
	printf("Enter number of strings to merge, followed by the strings\n");
	printf("> ");

	size_t strings_count;
	while (scanf("%zu", &strings_count) == 1) {
		size_t i;
		for (i = 0; i < strings_count; i++) {
			scanf("%s", buff);
			strings[i] = strdup(buff);
			assert(strings[i] != NULL);
		}

		printf("%u\n", optimal_merge(strings, strings_count));

		for (i = 0; i < strings_count; i++) {
			free(strings[i]);
		}

		printf("> ");
	}

	return 0;
}
