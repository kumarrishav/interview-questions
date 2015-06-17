/* Given a sorted array and a value n, count the number of pairs (i,j)
 * such that A[i]+A[j] >= n
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>

/* Find first index in arr[] s.t. arr[i] >= val */
static ssize_t binsrch_gte(int arr[], size_t arr_sz, int val) {
	ssize_t l = -1;
	ssize_t r = arr_sz;

	/* Conceptual assumption: arr[l] < val <= arr[r] */

	/* Invariant: arr[l] < val <= arr[r] */
	while (l+1 != r) {
		ssize_t mid = l+(r-l)/2;
		if (arr[mid] < val) {
			l = mid;
		} else {
			r = mid;
		}
	}

	return (l+1 == arr_sz) ? (-1) : (l+1);
}

size_t count_sum(int arr[], size_t arr_sz, int n) {
	size_t i;
	size_t res = 0;

	for (i = 0; i < arr_sz-1; i++) {
		ssize_t j = binsrch_gte(&arr[i+1], arr_sz-i-1, n-arr[i]);
		if (j != -1) {
			j += i+1;
			printf("Found: arr[%zu]+arr[%zd..%zu] = %d+%d = %d >= %d\n",
			       i, j, arr_sz-1, arr[i], arr[j], arr[i]+arr[j], n);
			res += arr_sz-j;
		}
	}

	return res;
}

static int array_buff[1024];

int main(void) {
	printf("Enter size of array, followed by elements, followed by N\n");
	printf("Note: array must be sorted\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buff[i]);
		}
		int n;
		scanf("%d", &n);
		printf("%zu\n", count_sum(array_buff, arr_sz, n));
		printf("> ");
	}

	return 0;
}
