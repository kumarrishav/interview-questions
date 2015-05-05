/* Given a sorted array of n integers that has been rotated an unknown number of times,
 * write code to find an element in the array. You may assume that the array
 * was originally sorted in increasing order.
 * EXAMPLE
 * Input: find 5 in { 15, 16, 19, 20, 25, 1, 3, 4, 5, 7, 10, 14 }
 * Output: 8 (the index of 5 in the array)
 *
 * Source: Cracking the Coding Interview, page 121, question 11.3
 */
#include <stdio.h>

static ssize_t bin_search(int arr[], ssize_t l, ssize_t r, int value) {
	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == value) {
			return m;
		} else if (arr[m] < value) {
			l = m+1;
		} else {
			r = m-1;
		}
	}

	return -1;
}

ssize_t find_on_rotated(int arr[], size_t arr_sz, int value) {
	if (arr_sz == 0) {
		return -1;
	}

	size_t i;
	for (i = 0; i+1 < arr_sz && arr[i] <= arr[i+1]; i++)
		; /* Intentionally left blank */

	if (arr[0] <= value && value <= arr[i]) {
		return bin_search(arr, 0, i, value);
	} else {
		return bin_search(arr, i+1, arr_sz-1, value);
	}
}

int main(void) {
	printf("Enter size of array, followed by the array elements, "
	       "followed by the element to find.\n");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		int arr[arr_sz];

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr[i]);
		}

		int to_find;
		scanf("%d", &to_find);

		ssize_t res = find_on_rotated(arr, arr_sz, to_find);
		if (res == -1) {
			printf("Element not found.\n");
		} else {
			printf("%d found at i = %zd\n", to_find, res);
		}
	}

	return 0;
}
