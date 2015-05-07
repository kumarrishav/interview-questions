/* Write a method to compute all permutations of a string.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.5
 */
#include <stdio.h>
#include <string.h>

void swap(char *arr, size_t i, size_t j) {
	char tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void str_permutations_aux(char *str, size_t str_sz, size_t str_i) {

	if (str_i == str_sz) {
		printf("Permutation: %s\n", str);
		return;
	}

	size_t i;
	for (i = str_i; i < str_sz; i++) {
		swap(str, str_i, i);
		str_permutations_aux(str, str_sz, str_i+1);
		swap(str, str_i, i);
	}
}

void str_permutations(char *str, size_t str_sz) {
	str_permutations_aux(str, str_sz, 0);
}

static char input[256];

int main(void) {
	printf("Enter a string to generate all permutations.\n");

	while (scanf("%s", input) == 1) {
		str_permutations(input, strlen(input));
	}

	return 0;
}
