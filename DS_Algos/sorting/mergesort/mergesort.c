#include <stdio.h>
#include <string.h>
#include <assert.h>

void mergesort_aux(int arr[], size_t low, size_t high, int helper[]) {

	if (low >= high) {
		return;
	}

	size_t middle = (low+high)/2;
	mergesort_aux(arr, low, middle, helper);
	mergesort_aux(arr, middle+1, high, helper);

	memcpy(&helper[low], &arr[low], sizeof(arr[0])*(high-low+1));

	size_t left_i = low;
	size_t right_i = middle+1;
	size_t cursor = low;

	while (left_i < middle+1 && right_i < high+1) {
		if (helper[left_i] < helper[right_i]) {
			arr[cursor] = helper[left_i];
			left_i++;
		} else {
			arr[cursor] = helper[right_i];
			right_i++;
		}
		cursor++;
	}

	while (left_i < middle+1) {
		arr[cursor++] = helper[left_i++];
	}
}

// This is an implementation for educational purposes only
// `aux` allocation will crash for very big arrays because of stack overflow
void mergesort(int arr[], size_t arr_sz) {
	assert(arr_sz > 0);
	int aux[arr_sz];
	mergesort_aux(arr, 0, arr_sz-1, aux);
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
		mergesort(array, array_sz);
		print_array(array, array_sz);
	}

	return 0;
}

