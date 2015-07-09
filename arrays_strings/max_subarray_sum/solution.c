/* In computer science, the maximum subarray problem is the task of finding the contiguous subarray
 * within a one-dimensional array of numbers (containing at least one positive number) which has the
 * largest sum. For example, for the sequence of values -2, 1, -3, 4, -1, 2, 1, -5, 4; the
 * contiguous subarray with the largest sum is 4, -1, 2, 1, with sum 6.
 *
 * Programming Pearls shows a simple linear solution:
 *
 * int max_sub_array_sum(int a[], int size) {
 *     int max_so_far = a[0], i;
 *     int curr_max = a[0];
 *
 *     for (i = 1; i < size; i++) {
 *         curr_max = max(a[i], curr_max+a[i]);
 *         max_so_far = max(max_so_far, curr_max);
 *     }
 *
 *     return max_so_far;
 * }
 *
 * Can you understand and explain how the algorithm works?
 * Modify the algorithm to return the start index of the maximum subarray
 * and its length.
 *
 * The prototype of the modified version should be:
 *
 * int max_sub_array_sum(int a[], int size, int *begin, int *len);
 *
 * EXAMPLE
 *
 * Using the earlier example, we know that the maximu subarray in [ -2, 1, -3, 4, -1, 2, 1, -5, 4 ]
 * is [ 4, -1, 2, 1 ] with sum 6. The modified version returns 6, and stores 3 in *begin (the index
 * of 4) and 4 in *len (the size of the maximum subarray).
 *
 * Source: Careercup (Adobe interview)
 */

#include <stdio.h>

int max_subarray_sum(int a[], size_t sz, size_t *begin, size_t *len) {
	int max_so_far = a[0];
	size_t max_so_far_start = 0;
	size_t max_so_far_len = 1;

	int curr_max = a[0];
	size_t curr_max_start = 0;
	size_t curr_max_len = 1;

	size_t i;
	for (i = 1; i < sz; i++) {
		if (a[i] > curr_max+a[i]) {
			curr_max = a[i];
			curr_max_start = i;
			curr_max_len = 1;
		} else {
			curr_max += a[i];
			curr_max_len++;
		}

		if (curr_max > max_so_far) {
			max_so_far = curr_max;
			max_so_far_start = curr_max_start;
			max_so_far_len = curr_max_len;
		}
	}

	*begin = max_so_far_start;
	*len = max_so_far_len;

	return max_so_far;
}

static int array[1024];

int main(void) {
	printf("Enter size of array, followed by the elements.\n");
	printf("> ");

	size_t array_sz;
	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array[i]);
		}

		size_t res_start;
		size_t res_len;
		int res;

		res = max_subarray_sum(array, array_sz, &res_start, &res_len);

		printf("max subarray sum = %d (a[%zd..%zd])\n", res, res_start, res_start+res_len-1);
		printf("> ");
	}

	return 0;
}
