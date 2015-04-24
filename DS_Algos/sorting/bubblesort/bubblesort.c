#include <stdio.h>

void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void bubblesort(int arr[], size_t arr_sz) {
	int swapped = 1;

	while (swapped) {
		swapped = 0;
		size_t i;
		for (i = 1; i < arr_sz; i++) {
			if (arr[i] < arr[i-1]) {
				swap(arr, i, i-1);
				swapped = 1;
			}
		}
	}

}

#define MAX_ARR_SZ 256
static int array[MAX_ARR_SZ];
static size_t array_sz;

static void print_array(int arr[], size_t arr_sz) {
	size_t i;
	for (i = 0; i < arr_sz; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int main(void) {
	printf("Input format: N n1 n2 n3 ...\n");
	printf("N is the array size; the rest are the elements\n");

	while (scanf("%zu", &array_sz) == 1) {
		size_t i;
		for (i = 0; i < array_sz; i++) {
			scanf("%d", &array[i]);
		}
		printf("Received: ");
		print_array(array, array_sz);
		printf("Sorted: ");
		bubblesort(array, array_sz);
		print_array(array, array_sz);
	}

	return 0;
}

