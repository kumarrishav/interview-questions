/* You are given an array consisting of elements where A[i+1] == A[i]+1 || A[i+1] == A[i]-1
 *
 * Write a function to find a given element in the array.
 *
 * EXAMPLE
 * Input: find 6 in [ 1, 2, 3, 4, 3, 4, 5, 6, 7 ]
 * Output: 7 (A[7] == 6)
 *
 * Source: Careercup (Amazon interview)
 */
#include <stdio.h>
#define abs(n) ((n) < 0 ? -(n) : (n))

/* The trick here is to skip positions where it would be impossible to find the given value.
 * For the above example, if we start at i = 0, we can see that we need 6-1 = 5 increments
 * In the best case, these 5 increments take place in the next 5 positions, so if A[0] == 1,
 * 6 can't appear before A[5].
 *
 * We keep doing this iteratively. This is still O(n) though (but it's slightly optimized)
 */
ssize_t find_elem(int arr[], size_t arr_sz, int val) {
	size_t i = 0;

	while (i < arr_sz) {
		if (arr[i] == val) {
			return i;
		}
		i += abs(val-arr[i]);
	}

	return -1;
}

static int array_buf[1024];
int main(void) {
	printf("Enter number of elements in array, followed by array elements, followed by value to find.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}
		int val;
		scanf("%d", &val);
		ssize_t res = find_elem(array_buf, arr_sz, val);
		if (res == -1) {
			printf("Not found.\n");
		} else {
			printf("Found at index %zd\n", res);
		}
		printf("> ");
	}

	return 0;
}
