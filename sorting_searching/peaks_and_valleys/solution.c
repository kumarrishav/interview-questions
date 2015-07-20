/* In an array of integers, a "peak" is an element which is greater than or equal to the adjacent
 * integers and a "valley" is an element which is less than or equal to the adjacent integers. For
 * example, in the array [ 5, 8, 6, 2, 3, 4, 6 ], [ 8, 6 ] are peaks and [ 5, 2 ] are valleys. Given
 * an array of integers, sort the array into an alternating sequence of peaks and valleys.
 *
 * EXAMPLE
 *
 * Input:  [ 5, 3, 1, 2, 3 ]
 * Output: [ 5, 1, 3, 2, 3 ]
 *         (or any other valid arrangement)
 *
 * Source: Cracking the Coding Interview, 6th Ed., Exercise 10.11, page 151
 */

#include <stdio.h>

void peaks_valleys(int arr[], size_t arr_sz) {
	size_t i;
	for (i = 1; i < arr_sz; i+= 2) {
		size_t largest_i = i;
		if (i > 0 && arr[i-1] > arr[largest_i])
			largest_i = i-1;
		if (i+1 < arr_sz && arr[i+1] > arr[largest_i])
			largest_i = i+1;
		int tmp = arr[largest_i];
		arr[largest_i] = arr[i];
		arr[i] = tmp;
	}
}

static void print_arr(int arr[], size_t sz) {
	printf("[ ");
	if (sz > 0)
		printf("%d", arr[0]);
	size_t i;
	for (i = 1; i < sz; i++)
		printf(", %d", arr[i]);
	printf(" ]");
}

int main(void) {
	static int arr_buff[1024];
	size_t arr_sz;

	printf("Enter array size, followed by the elements.\n");
	printf("> ");

	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++)
			scanf("%d", &arr_buff[i]);
		peaks_valleys(arr_buff, arr_sz);
		print_arr(arr_buff, arr_sz);
		printf("\n> ");
	}

	return 0;
}
