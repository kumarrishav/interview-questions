/* Rearrange a matrix such that rows are in ascending order and columns are in descending order.
 *
 * EXAMPLE
 *
 * For the following 3x2 matrix:
 *
 * [  9 4 ]
 * [ 10 5 ]
 *
 * The result is:
 *
 * [ 9 10 ]
 * [ 4  5 ]
 *
 * For bigger matrices, there may be numerous possibilities. Any rearrangement that fulfills the
 * requirements is fine.
 *
 * Source: Careercup (Amazon interview)
 */

#include <stdio.h>

static void swap(int arr[], size_t i, size_t j) {
	int tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static void swap_arrays(int arr1[], int arr2[], size_t sz) {
	size_t i;
	for (i = 0; i < sz; i++) {
		int tmp = arr1[i];
		arr1[i] = arr2[i];
		arr2[i] = tmp;
	}
}

static void quicksort(int arr[], size_t sz) {
	if (sz <= 1) {
		return;
	}

	swap(arr, 0, sz/2);

	size_t i, j;
	i = 0;
	for (j = i+1; j < sz; j++) {
		if (arr[j] <= arr[0]) {
			i++;
			swap(arr, i, j);
		}
	}

	swap(arr, 0, i);
	quicksort(arr, i);
	quicksort(arr+i+1, sz-(i+1));
}

void matrix_mostly_sorted(size_t rows, size_t cols, int matrix[rows][cols]) {
	quicksort(&matrix[0][0], rows*cols);
	size_t i;
	for (i = 0; i < rows/2; i++) {
		swap_arrays(&matrix[i][0], &matrix[rows-(i+1)][0], cols);
	}
}

static void print_matrix(size_t rows, size_t cols, int matrix[rows][cols]) {
	size_t i, j;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			printf("%d\t", matrix[i][j]);
		}
		putchar('\n');
	}
}

int main(void) {
	printf("Enter matrix dimensions followed by the elements\n");
	printf("> ");

	size_t rows, cols;
	while (scanf("%zu%zu", &rows, &cols) == 2) {
		int matrix[rows][cols];
		size_t i, j;
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				scanf("%d", &matrix[i][j]);
			}
		}

		matrix_mostly_sorted(rows, cols, matrix);

		print_matrix(rows, cols, matrix);
		printf("> ");
	}

	return 0;
}
