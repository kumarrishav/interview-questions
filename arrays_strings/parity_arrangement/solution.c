
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
