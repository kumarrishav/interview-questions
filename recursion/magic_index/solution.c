/* A magic index in an array A[0..n-1] is defined to be an index such that A[i] = i.
 * Given a sorted array of distinct integers, write a method to find a magic
 * index, if one exists, in array A.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.3
 */
#include <stdio.h>

ssize_t magic_index(int arr[], size_t arr_sz) {
	ssize_t l = 0;
	ssize_t r = arr_sz-1;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == m) {
			return m;
		} else if (arr[m] > m) {
			r = m-1;
		} else {
			l = m+1;
		}
	}

	return -1;
}

static int array[256];
size_t array_sz;

int main(void) {
	printf("Enter array size, followed by array elements\n");

	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array[i]);
		}

		ssize_t magic_i = magic_index(array, array_sz);

		if (magic_i == -1) {
			printf("No magic index\n");
		} else {
			printf("Magic index = %zd\n", magic_i);
		}
	}

	return 0;
}
