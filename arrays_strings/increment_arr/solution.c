/* You are given an array representing an integer. Write a function which increments this integer.
 *
 * EXAMPLE
 *
 * Input:  [ 1, 2, 3 ] (represents the number 123)
 * Result: [ 1, 2, 4 ] (123+1 = 124)
 *
 * Source: Careercup (Google interview)
 */

#include <stdio.h>

int incr_array(int arr[], size_t arr_sz) {
	ssize_t i = arr_sz;
	i--;

	while (i >= 0 && arr[i] == 9) {
		arr[i] = 0;
		i--;
	}

	if (i >= 0) {
		arr[i]++;
		return 1;
	} else {
		return 0;
	}
}

static void print_array(int arr_buff[], size_t arr_sz) {
	printf("[ ");

	if (arr_sz > 0) {
		printf("%d", arr_buff[0]);
	}

	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr_buff[i]);
	}

	printf(" ]\n");
}

static int arr_buff[1024];

int main(void) {
	printf("Enter the size of the array, followed by its elements\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr_buff[i]);
		}
		print_array(arr_buff, arr_sz);
		while (incr_array(arr_buff, arr_sz) == 1) {
			print_array(arr_buff, arr_sz);
		}
		printf("> ");
	}

	return 0;
}
