/* Write a function that returns the max k numbers from an unsorted integer array.
 * Each number in the array is in the range [0, 10000).
 *
 * Source: Glassdoor (Google interview)
 */
#include <stdio.h>
#include <string.h>
#define RANGE_MIN 0
#define RANGE_MAX 10000

size_t max_k(size_t k, unsigned result[k], size_t n, unsigned arr[n]) {
	static unsigned count[RANGE_MAX-RANGE_MIN];
	memset(count, 0, sizeof(count));

	ssize_t i;
	for (i = 0; i < n; i++) {
		count[arr[i]-RANGE_MIN]++;
	}

	size_t res_i = 0;
	for (i = RANGE_MAX-RANGE_MIN-1; res_i < k && i >= 0; i--) {
		size_t j;
		for (j = 0; res_i < k && j < count[i]; j++) {
			result[res_i++] = i+RANGE_MIN;
		}
	}

	return res_i;
}

int main(void) {
	printf("Enter the size of the array and the value of k, followed by the array elements.\n"
	       "Use only elements in the range [%d, %d)\n", RANGE_MIN, RANGE_MAX);
	printf("> ");

	size_t arr_sz;
	size_t k;
	while (scanf("%zu%zu", &arr_sz, &k) == 2) {

		unsigned array[arr_sz];
		unsigned result[k];

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%u", &array[i]);
		}

		size_t written = max_k(k, result, arr_sz, array);
		printf("Max %zu = [ ", k);
		if (written > 0) {
			printf("%u", result[0]);
		}
		for (i = 1; i < written; i++) {
			printf(", %u", result[i]);
		}
		printf(" ];\n> ");
	}

	return 0;
}
