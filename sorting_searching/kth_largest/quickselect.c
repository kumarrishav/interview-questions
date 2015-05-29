/* Write a function that finds and returns the k-th largest element of an array.
 *
 * Source: Careercup (Amazon interview)
 */
#include <stdio.h>

static void swap(int arr[], size_t i, size_t j) {
	int tmp;
	tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static ssize_t kth_largest_aux(int arr[], size_t arr_sz, size_t low, size_t high, size_t k) {
	size_t pi = low+(high-low)/2;

	size_t i = low;
	swap(arr, i, pi);

	size_t j;
	for (j = i+1; j < high; j++) {
		if (arr[j] < arr[low]) {
			i++;
			swap(arr, i, j);
		}
	}

	swap(arr, low, i);

	if (arr_sz-i == k) {
		return i;
	} else if (arr_sz-i > k) {
		return kth_largest_aux(arr, arr_sz, i+1, high, k);
	} else {
		return kth_largest_aux(arr, arr_sz, low, i, k);
	}
}

ssize_t kth_largest(int arr[], size_t arr_sz, size_t k) {
	if (k > arr_sz) {
		return -1;
	} else {
		return kth_largest_aux(arr, arr_sz, 0, arr_sz, k);
	}
}

int main(void) {
	printf("Enter array size, followed by each element, followed by K\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		int array[arr_sz];

		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array[i]);
		}

		size_t k;
		scanf("%zu", &k);

		ssize_t res;
		res = kth_largest(array, arr_sz, k);

		if (res == -1) {
			printf("Invalid K. Please choose a value >= 1 and <= array_size\n");
		} else {
			printf("%zu-th largest = %d\n", k, array[res]);
		}

		printf("> ");
	}

	return 0;
}
