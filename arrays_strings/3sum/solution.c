/* Given an array of integers, write a function to check if any 3 integers in that
 * array sum to 0
 *
 * Source: Glassdoor (Facebook interview)
 */
#include <stdio.h>

// A good opportunity to review quicksort

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static size_t partition(size_t N, int arr[N], size_t pivot) {
	swap(arr, 0, pivot);
	size_t i = 0;
	size_t j = 1;

	for (; j < N; j++) {
		if (arr[j] < arr[0]) {
			i++;
			swap(arr, i, j);
		}
	}

	swap(arr, 0, i);

	return i;
}

static void quicksort(size_t N, int arr[N]) {
	if (N <= 1) {
		return;
	}

	size_t mid = N/2;
	size_t p = partition(N, arr, mid);

	quicksort(p, arr);
	quicksort(N-p-1, arr+p+1);
}

int sum_to_zero(size_t N, int arr[N], size_t *i, size_t *j, size_t *k) {

	quicksort(N, arr);

	size_t arr_i = 0;
	for (arr_i = 0; arr_i < N; arr_i++) {
		size_t front_cursor = arr_i+1;
		size_t back_cursor = N-1;
		while (front_cursor < back_cursor) {
			int sum = arr[arr_i] + arr[front_cursor] + arr[back_cursor];
			if (sum == 0) {
				*i = arr_i;
				*j = front_cursor;
				*k = back_cursor;
				return 1;
			} else if (sum < 0) {
				front_cursor++;
			} else {
				back_cursor--;
			}
		}
	}

	return 0;
}

static int input_arr[512];
static size_t input_sz;

int main(void) {
	printf("Enter number of elements, followed by the elements\n");
	printf("> ");

	while (scanf("%zu", &input_sz) == 1) {
		size_t i;
		for (i = 0; i < input_sz; i++) {
			scanf("%d", &input_arr[i]);
		}

		size_t a, b, c;
		if (sum_to_zero(input_sz, input_arr, &a, &b, &c)) {
			printf("%d+%d+%d = 0\n", input_arr[a], input_arr[b], input_arr[c]);
		} else {
			printf("Not solvable\n");
		}

		printf("> ");
	}

	return 0;
}
