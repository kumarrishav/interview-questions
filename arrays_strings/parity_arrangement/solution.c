/* You are given an array of even length that has the same number of odd elements and even elements.
 *
 * Write a function that rearranges the array elements such that every even element is stored in an
 * odd index, and every odd element is stored in an even index.
 *
 * EXAMPLE
 *
 * Input:
 * [ 2, 1, 3, 4, 7, 9, 24, 98 ]
 *
 * Output:
 * [ 1, 2, 3, 4, 7, 24, 9, 98 ] (or any other order, as long as the indexes rule isn't broken)
 *
 * Source: Careercup (VMWare interview)
 */
#include <stdio.h>

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void parity_rearrange(int arr[], size_t n) {
	size_t even;
	size_t odd;

	odd = 0;
	even = 1;

	while (odd < n && (arr[odd] & 1)) {
		odd += 2;
	}
	while (even < n && !(arr[even] & 1)) {
		even += 2;
	}

	while (odd < n && even < n) {
		swap(arr, odd, even);
		while (odd < n && (arr[odd] & 1)) {
			odd += 2;
		}
		while (even < n && !(arr[even] & 1)) {
			even += 2;
		}
	}
}

static void print_array(int arr[], size_t arr_sz) {
	printf("[");
	if (arr_sz > 0) {
		printf(" %d", arr[0]);
	}

	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr[i]);
	}

	printf(" ]\n");
}

int main(void) {
	printf("Enter length of array, followed by the elements.\n");
	printf("Note: there must be the same number of even and odd elements.\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		int arr[arr_sz];
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &arr[i]);
		}
		parity_rearrange(arr, arr_sz);
		print_array(arr, arr_sz);
		printf("> ");
	}

	return 0;
}
