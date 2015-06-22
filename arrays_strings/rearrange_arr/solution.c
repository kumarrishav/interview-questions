
#include <stdio.h>

void rearrange(size_t arr_sz, int arr[]) {
	size_t i;
	for (i = 0; i < arr_sz; i++) {
		arr[i] += (arr[arr[i]]%arr_sz)*arr_sz;
	}
	for (i = 0; i < arr_sz; i++) {
		arr[i] /= arr_sz;
	}
}

static void print_array(size_t arr_sz, int arr[]) {
	printf("[ ");
	if (arr_sz > 0) {
		printf("%d", arr[0]);
	}
	size_t i;
	for (i = 1; i < arr_sz; i++) {
		printf(", %d", arr[i]);
	}
	printf(" ]");
}

static int array_buf[1024];

int main(void) {
	printf("Enter array length, followed by each element\n");
	printf("Note that for an array of size N, elements can only be in the range [0..N-1]\n");
	printf("> ");

	size_t arr_sz;
	while (scanf("%zu", &arr_sz) == 1) {
		size_t i;
		for (i = 0; i < arr_sz; i++) {
			scanf("%d", &array_buf[i]);
		}
		printf("Input: ");
		print_array(arr_sz, array_buf);

		rearrange(arr_sz, array_buf);

		printf("\nOutput: ");
		print_array(arr_sz, array_buf);
		printf("\n> ");
	}

	return 0;
}
