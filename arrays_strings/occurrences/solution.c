/* Given a sorted array, write a function to find the number of occurrences of a number
 * in that array
 *
 * EXAMPLE
 *
 * Input array: [ 1, 1, 2, 3, 3, 3, 4, 5 ]
 * Input value: 3
 * Output:      3 (the number 3 occurs 3 times)
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>
#include <assert.h>

static ssize_t bin_search_first(int arr[], size_t n, int val) {
	ssize_t l = -1;
	ssize_t r = n;

	while (l+1 != r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] < val) {
			l = m;
		} else {
			r = m;
		}
	}

	if (r >= n || arr[r] != val) {
		return -1;
	} else {
		return r;
	}
}

static ssize_t bin_search_last(int arr[], size_t n, int val) {
	ssize_t l = -1;
	ssize_t r = n;

	while (l+1 != r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] <= val) {
			l = m;
		} else {
			r = m;
		}
	}

	if (l == -1 || arr[l] != val) {
		return -1;
	} else {
		return l;
	}
}

ssize_t occurrences(int arr[], size_t n, int val) {
	ssize_t first = bin_search_first(arr, n, val);

	if (first == -1) {
		return -1;
	}

	ssize_t last = bin_search_last(arr, n, val);
	assert(last != -1);

	return last-first+1;
}

static int arr_buff[1024];

int main(void) {
	printf("Enter array size, followed by the elements, followed by the element to count occurrences\n");
	printf("Note: array must be sorted\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr_buff[i]);
		}

		int target;
		scanf("%d", &target);

		ssize_t res = occurrences(arr_buff, arr_sz, target);
		if (res == -1) {
			printf("%d does not occur in the array\n", target);
		} else {
			printf("%d occurs %zd times in the array\n", target, res);
		}

		printf("> ");
	}

	return 0;
}
