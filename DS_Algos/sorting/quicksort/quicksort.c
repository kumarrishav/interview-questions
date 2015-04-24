#include <stdio.h>
#include <string.h>
#include <assert.h>

void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

size_t partition(int arr[], size_t low, size_t high, size_t pivot) {
	assert(low <= pivot && pivot < high);

	swap(arr, low, pivot);
	size_t cursor = low+1;

	size_t i;
	for (i = low+1; i < high; i++) {
		if (arr[i] <= arr[low]) {
			swap(arr, i, cursor);
			cursor++;
		}
	}

	cursor--;
	swap(arr, low, cursor);

	return cursor;
}

void quicksort_aux(int arr[], size_t low, size_t high) {
	if (high < low || high-low <= 1)
		return;

	size_t pivot = (low+high)/2;

	pivot = partition(arr, low, high, pivot);
	quicksort_aux(arr, low, pivot);
	quicksort_aux(arr, pivot+1, high);
}

void quicksort(int arr[], size_t arr_sz) {
	quicksort_aux(arr, 0, arr_sz);
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
		quicksort(array, array_sz);
		print_array(array, array_sz);
	}

	return 0;
}

