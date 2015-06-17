/* Given a sorted array with only 0s and 1s, count the number of 0s.
 *
 * EXAMPLE
 *
 * Input: [ 0, 0, 0, 0, 1, 1 ]
 * Output: 4
 *
 * Source: Careercup (Amazon interview)
 */
#include <stdio.h>

size_t zero_count(int arr[], size_t arr_sz) {
	ssize_t l = -1;
	ssize_t r = arr_sz;

	/* Conceptual assumption: arr[-1] <= 0 < arr[arr_sz] */

	/* Loop invariant: arr[l] <= 0 < arr[r] */
	while (l+1 != r) {
		ssize_t mid = l+(r-l)/2;
		if (arr[mid] <= 0) {
			l = mid;
		} else {
			r = mid;
		}
	}

	return r;
}

static void print_array(int arr[], size_t arr_sz) {
	printf("[ ");
	if (arr_sz > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ]\n");
}

static int arr_buff[1024];

int main(void) {
	printf("Enter size of array, followed by the elements.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr_buff[i]);
		}
		printf("Input:\n");
		print_array(arr_buff, arr_sz);
		printf("Number of zeros: %zd\n", zero_count(arr_buff, arr_sz));
		printf("> ");
	}

	return 0;
}
