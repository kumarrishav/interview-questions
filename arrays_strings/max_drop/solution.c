/* Given an array with at least 2 integers, write a function that finds the maximum difference
 * between a pair of elements A[m] and A[n], where m < n.
 *
 * EXAMPLE
 *
 * Input:  [ 10, 6, 7, 8, 1, 5, 20 ]
 *
 * Output: 9
 *
 * Explanation: the maximum difference is achieved with m = 0 and n = 4, because A[0]-A[4] = 9,
 *              and there's no other pair (i, j) such that i < j and A[i]-A[j] > 9
 *
 * Source: Careercup (Facebook interview)
 */

#include <stdio.h>
#include <assert.h>

/* This problem can be solved in linear time. We do so by reducing it to the maximum subarray
 * problem as follows: for an input array A of size n, compute an auxiliary array, B, of size
 * n-1 such that B[i] = A[i]-A[i+1].
 *
 * Thus the difference between any two elements A[i] and A[j]
 * is given by B[i] + B[i+1] + B[i+2] + ... + B[j-1].
 *
 * So, we basically want to find the maximum subarray sum, which represents the maximum difference
 * between a pair of elements.
 *
 * Since there is a known O(n) algorithm that solves the maximum subarray problem, we can solve this
 * in O(n). Also, the maximum subarray sum solution traverses the array left-to-right, and reads
 * each entry only once, so we don't need to explicitly build B. Instead, we generate the next
 * value that would be in B on each iteration (it's just A[i]-A[i+1]).
 *
 * So we get O(n) time and O(1) memory. It can't get better than that.
 */

#define max(a, b) ((a) > (b) ? (a) : (b))

int max_drop(int arr[], size_t len) {
	assert(len >= 2);
	int max_so_far = arr[0]-arr[1];
	int curr_max = max_so_far;

	size_t i;
	for (i = 1; i < len-1; i++) {
		curr_max = max(arr[i]-arr[i+1], curr_max+arr[i]-arr[i+1]);
		max_so_far = max(max_so_far, curr_max);
	}

	return max_so_far;
}

static int arr[1024];

int main(void) {
	printf("Enter size of array, followed by the elements\n");
	printf("> ");

	size_t arr_len;
	while (scanf("%zu", &arr_len) == 1) {
		size_t i;
		for (i = 0; i < arr_len; i++)
			scanf("%d", &arr[i]);
		if (arr_len < 2)
			printf("Arrays must have at least 2 elements.\n");
		else
			printf("Max drop = %d\n", max_drop(arr, arr_len));
		printf("> ");
	}

	return 0;
}
