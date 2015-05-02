#include <stdio.h>
#define MAX_INPUT_SZ 256

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

			ssize_t res = binary_search(sorted_arr, arr_sz, val);
			if (res == -1) {
				printf("%d not found\n", val);
			} else {
				printf("%d found at i = %zd\n", val, res);
			}
		}
	}

	return 0;
}
