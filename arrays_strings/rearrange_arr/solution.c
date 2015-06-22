/* Given an array of size N with elements in the range [0..N-1],
 * rearrange the array such that A[i] = A[A[i]].
 *
 * Discuss the different approaches. Strive for an O(N) algorithm using O(1) memory.
 * Assume that the result of N*N never overflows.
 *
 * EXAMPLE
 *
 * Input: A = [ 3, 0, 2, 1 ]
 * Output: A = [ 1, 3, 2, 0 ]
 *
 * Explanation:
 * A[0] stores the element that was in A[A[0]] = A[3] = 1
 * A[1] stores the element that was in A[A[1]] = A[0] = 3
 * A[2] stores the element that was in A[A[2]] = A[2] = 2
 * A[3] stores the element that was in A[A[3]] = A[1] = 0
 *
 * Source: Careercup (Facebook interview)
 */
#include <stdio.h>

/* This deserves some explaining. The problem would be easy if we were allowed to use O(N) memory.
 * We must use O(1) memory, yet, as the example shows, new positions are set according to the
 * original value of each position. For example, A[1], that stored 0, became 3, but later on
 * A[3] became 0, which was the old value in A[1]. How can we even know the old value on A[1]
 * if we overwrite it? How is it possible to use constant memory if the new array needs the
 * old values?
 *
 * In problems that seem impossible, such as this one, the key is usually to somehow reuse the space
 * that stores the input. The code below does precisely this: each position is first assigned a new
 * number that represents the old value and the new value. The encoding stores the number that
 * represents the concatenation of the new value followed by the old value in base-N.
 *
 * Then, a second pass is needed to eliminate all the old values. Since old values are stored as the
 * least significant digit in base-N, we just have to divide by N to remove it.
 *
 * For example, with N = 100, after the first pass, one entry in the array may look like this: 3941.
 * This means that the new value will be 39 and the old value was 41.
 *
 * Beautiful, right?
 */
void rearrange(size_t arr_sz, int arr[]) {
	size_t i;
	for (i = 0; i < arr_sz; i++) {
		arr[i] += (arr[arr[i]]%arr_sz)*arr_sz;
	}
	for (i = 0; i < arr_sz; i++) {
		arr[i] /= arr_sz;
	}
}

static void print_array(size_t arr_sz, int arr[]) {
	printf("[ ");
	if (arr_sz > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ]");
}

static int array_buf[1024];

int main(void) {
	printf("Enter array length, followed by each element\n");
	printf("Note that for an array of size N, elements can only be in the range [0..N-1]\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}
		printf("Input: ");
		print_array(arr_sz, array_buf);

		rearrange(arr_sz, array_buf);

		printf("\nOutput: ");
		print_array(arr_sz, array_buf);
		printf("\n> ");
	}

	return 0;
}
