/* This file implements the traditional binary search algorithm,
 * along with some interesting variants:
 *
 * - binary_search() - traditional binary search
 * - bin_search_closest() - returns the closest match if the element isn't found
 * - bin_search_first_occurrence - if there are duplicates, returns the first occurrence
 * - bin_search_last_occurrence - if there are duplicates, returns the last occurrence
 *
 * This is actually quite important. In one of my interviews, I had to develop an algorithm
 * for an interesting problem where one of the steps required the use of bin_search_closest().
 * Needless to say, the interviewer wanted me to code it. It is always important to know
 * how binary search works and how it can be modified to suit our specific needs.
 */
#include <stdio.h>
#include <assert.h>
#define MAX_INPUT_SZ 256
#define abs(x) ((x) >= 0 ? (x) : -(x))

/* Traditional binary search */
ssize_t binary_search(int arr[], size_t arr_sz, int val) {
	ssize_t l = 0, r = arr_sz-1;
	ssize_t mid;

	while (l <= r) {
		mid = l+(r-l)/2;
		if (arr[mid] == val) {
			return mid;
		}
		if (arr[mid] < val) {
			l = mid+1;
		} else {
			r = mid-1;
		}
	}

	return -1;
}

/* This modified binary search returns the closest element to `val`
 * if `val` is not in the array
 */
ssize_t bin_search_closest(int arr[], size_t arr_sz, int val) {
	ssize_t l = 0, r = arr_sz-1;
	ssize_t mid;

	while (l <= r) {
		mid = l+(r-l)/2;
		if (arr[mid] == val) {
			return mid;
		}
		if (arr[mid] < val) {
			l = mid+1;
		} else {
			r = mid-1;
		}
	}

	if (l < arr_sz && r >= 0) {
		int diff_l = abs(val-arr[l]);
		int diff_r = abs(val-arr[r]);
		return diff_l < diff_r ? l : r;
	} else if (l == arr_sz) {
		return r;
	} else if (r < 0) {
		return l;
	} else {
		assert(0);
	}
}

/* This modified version returns the first occurrence of `val` in `arr`.
 * It is equivalent to the traditional binary search if `arr` doesn't have duplicates
 *
 * Based on the pseudo-code on Column 9, Section 9.3 (page 93) of Programming Pearls 2nd edition
 */
ssize_t bin_search_first_occurrence(int arr[], size_t arr_sz, int val) {

	/* Conceptual assumption: arr[-1] < val <= arr[arr_sz]
	 * The code never actually accesses those positions, but this
	 * conceptual assumption is important to validate the loop invariant
	 */
	ssize_t l = -1;
	ssize_t r = arr_sz;

	/* invariant: arr[l] < val <= arr[r] */
	while (l+1 != r) {
		ssize_t m = l+(r-l)/2;
		if (arr[m] < val) {
			l = m;
		} else {
			r = m;
		}
	}

	if (r == arr_sz || arr[r] != val) {
		return -1;
	} else {
		return r;
	}
}

/* This modified version returns the last occurrence of `val` in `arr`.
 * It is equivalent to the traditional binary search if `arr` doesn't have duplicates
 *
 * Based on the pseudo-code on Column 9, Section 9.3 (page 93) of Programming Pearls 2nd edition
 */
ssize_t bin_search_last_occurrence(int arr[], size_t arr_sz, int val) {

	/* Conceptual assumption: arr[-1] <= val < arr[arr_sz]
	 * The code never actually accesses those positions, but this
	 * conceptual assumption is important to validate the loop invariant
	 */
	ssize_t l = -1;
	ssize_t r = arr_sz;

	/* invariant: arr[l] <= val < arr[r] */
	while (l+1 != r) {
		ssize_t m = l+(r-l)/2;
		if (val < arr[m]) {
			r = m;
		} else {
			l = m;
		}
	}

	if (l == arr_sz || arr[l] != val) {
		return -1;
	} else {
		return l;
	}
}

static int sorted_arr[MAX_INPUT_SZ];
int main(void) {
	printf("Enter number of elements in array, followed by the elements (sorted), "
	       "followed by the number of queries, followed by each query.\n");

	size_t arr_sz;

	while (scanf("%zu", &arr_sz) == 1) {

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &sorted_arr[i]);
		}

		size_t queries;
		scanf("%zu", &queries);

		for (i = 0; i < queries; i++) {
			int val;
			scanf("%d", &val);

			printf("Traditional binary search: ");
			ssize_t res = binary_search(sorted_arr, arr_sz, val);
			if (res == -1) {
				printf("%d not found\n", val);
			} else {
				printf("%d found at i = %zd\n", val, res);
			}

			printf("First occurrence: ");
			ssize_t first = bin_search_first_occurrence(sorted_arr, arr_sz, val);
			if (first == -1) {
				printf("%d not found\n", val);
			} else {
				printf("%d occurs for the first time at i = %zd\n", val, first);
			}

			printf("Last occurrence: ");
			ssize_t last = bin_search_last_occurrence(sorted_arr, arr_sz, val);
			if (last == -1) {
				printf("%d not found\n", val);
			} else {
				printf("%d occurs for the last time at i = %zd\n", val, last);
			}

			printf("Closest match: ");
			res = bin_search_closest(sorted_arr, arr_sz, val);
			printf("%d\n\n", sorted_arr[res]);
		}
	}

	return 0;
}
