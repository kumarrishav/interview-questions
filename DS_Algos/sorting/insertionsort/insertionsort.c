#include <stdio.h>

void insertionsort(int arr[], size_t arr_sz) {
        size_t i, j;
	for (i = 1; i < arr_sz; i++) {
		j = i;
		int v = arr[j];
		while (j > 0 && v < arr[j-1]) {
			arr[j] = arr[j-1];
			j--;
		}
		arr[j] = v;
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
		insertionsort(array, array_sz);
		print_array(array, array_sz);
	}

	return 0;
}

