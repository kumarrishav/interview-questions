/* Write a method to return all subsets of a set.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.4
 */
#include <stdio.h>

void print_subsets_aux(char *buff, size_t buff_i, size_t n, int set[n], size_t set_i) {
	if (set_i == n) {
		sprintf(buff+buff_i, "};");
		printf("Subset: %s\n", buff);
		return;
	}

	print_subsets_aux(buff, buff_i, n, set, set_i+1);
	int written = sprintf(buff+buff_i, "%d ", set[set_i]);
	print_subsets_aux(buff, buff_i+written, n, set, set_i+1);
}

void print_subsets(size_t n, int set[n]) {
	// Assumption: printing each element with a space takes at most 4 characters
	char buff[n*4+5];
	print_subsets_aux(buff, sprintf(buff, "{ "), n, set, 0);
}

int main(void) {
	printf("Enter set size, followed by set elements\n");

	size_t set_size;
	while (scanf("%zu", &set_size) == 1) {
		int set[set_size];

		size_t i;
		for (i = 0; i < set_size; i++) {
			scanf("%d", &set[i]);
		}

		print_subsets(set_size, set);
	}

	return 0;
}
