
#include <stdio.h>

void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

void sort_modulo3(int arr[], size_t n) {
	size_t i, j;
	for (i = 0, j = 0; j < n; j++) {
		if (arr[j]%3 == 0) {
			swap(arr, i, j);
			i++;
		}
	}
	for (j = i; j < n; j++) {
		if (arr[j]%3 == 1) {
			swap(arr, i, j);
			i++;
		}
	}
}

static void print_array(int arr[], size_t arr_sz) {
	printf("arr = [ ");
	if (arr_sz > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ];\n");
}

static int array[1024];

int main(void) {
	printf("Enter size of array, followed by the elements\n");
	printf("> ");

	size_t elems;
	while (scanf("%zu", &elems) == 1) {
		size_t i;
		for (i = 0; i < elems; i++) {
			scanf("%d", &array[i]);
		}

		sort_modulo3(array, elems);
		print_array(array, elems);

		printf("> ");
	}

	return 0;
}
