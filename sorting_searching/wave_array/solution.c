/* A wave array is an array that starts off with an increasing sequence of values up to an unknown
 * index i, followed by a decreasing sequence of values.
 *
 * For example, this is a wave array:
 *
 * A = [ 1, 2, 5, 8, 13, 9, 3, -1 ]
 *
 * Because the values in A[0..4] are increasing and the values in A[4..7] are decreasing.
 *
 * Write a function that finds an element in the array. If the element is there, the function
 * returns the index of that element, otherwise, it returns -1.
 *
 * Avoid linear scans.
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <assert.h>

static size_t find_max(size_t arr_sz, int arr[arr_sz]) {

	if (arr_sz <= 1) {
		return 0;
	}

	ssize_t l = 0;
	ssize_t r = arr_sz;
	ssize_t m = l+(r-l)/2;

	while (m < arr_sz-1 && (arr[m-1] >= arr[m] || arr[m+1] >= arr[m])) {
		assert(m > 0);
		if (arr[m-1] > arr[m]) {
			r = m;
		} else {
			l = m+1;
		}
		m = l+(r-l)/2;
	}

	if (m == arr_sz-1) {
		return m-1;
	} else {
		return m;
	}
}

ssize_t find_elem(size_t arr_sz, int arr[arr_sz], int val) {
	size_t max = find_max(arr_sz, arr);

	ssize_t l;
	ssize_t r;

	// Search on left
	l = 0;
	r = max;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == val) {
			return m;
		} else if (arr[m] < val) {
			l = m+1;
		} else {
			r = m-1;
		}
	}

	// Search on right
	l = max;
	r = arr_sz-1;

	while (l <= r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] == val) {
			return m;
		} else if (arr[m] < val) {
			r = m-1;
		} else {
			l = m+1;
		}
	}

	return -1;
}

static int array_buf[1024];

int main(void) {
	printf("Enter array size, followed by the elements, followed by element to find\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}

		int val;
		scanf("%d", &val);
		ssize_t res = find_elem(arr_sz, array_buf, val);

		if (res == -1) {
			printf("Not found.\n");
		} else {
			printf("Found at array[%zd]\n", res);
		}

		printf("> ");
	}

	return 0;
}
