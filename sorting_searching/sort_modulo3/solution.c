/* Write a function to sort an array modulo 3, that is, in increasing order of array[i]%3
 * Can you do it in O(N) time and O(1) memory?
 *
 * EXAMPLE
 *
 * Input:  [ 1, 2, 3, 4, 5 ]
 * Output: [ 3, 1, 4, 2, 5 ] (or other valid ordering)
 *
 * Explanation:
 * 1%3 = 1
 * 2%3 = 2
 * 3%3 = 0
 * 4%3 = 1
 * 5%3 = 2
 *
 * Thus, the sorted sequence modulo 3 will be [ 0, 1, 1, 2, 2 ]
 * So, the possible solutions are:
 *
 * [ 3, 1, 4, 2, 5 ]
 * [ 3, 1, 4, 5, 2 ]
 * [ 3, 4, 1, 2, 5 ]
 * [ 3, 4, 1, 5, 2 ]
 *
 * Source: Careercup
 */
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
