/* Write a function in C called my2DAlloc which allocates a two-dimensional
 * array. Minimize the number of calls to malloc and make sure that the memory
 * is accessible by the notation arr[i][j].
 *
 * Source: Cracking the Coding Interview, page 140, quetion 13.10
 */
#include <stdio.h>
#include <stdlib.h>

int **two_d_alloc(size_t rows, size_t cols) {
	int **arr = malloc(rows*sizeof(*arr) + rows*cols*sizeof(**arr));

	if (arr == NULL) {
		return NULL;
	}

	size_t i;
	for (i = 0; i < rows; i++) {
		arr[i] = (int *)(arr+rows)+i*cols;
	}

	return arr;
}

int main(void) {
	printf("Enter the array dimensions\n");
	size_t rows, cols;
	scanf("%zu%zu", &rows, &cols);

	int **arr = two_d_alloc(rows, cols);
	if (arr == NULL) {
		fprintf(stderr, "Out of memory.\n");
		return 127;
	}

	printf("Available commands are:\n"
	       "W i j n - Write the value `n` into position (i, j)\n"
	       "R i j - Read and print the value in position (i, j)\n"
	       "Q - Quit\n");

	while (1) {
		size_t row, col;
		int val;
		if (scanf(" W %zu%zu%d", &row, &col, &val) == 3) {
			arr[row][col] = val;
			printf("Stored %d in (%zu, %zu)\n", val, row, col);
		} else if (scanf(" R %zu%zu", &row, &col) == 2) {
			printf("(%zu, %zu) = %d\n", row, col, arr[row][col]);
		} else {
			break;
		}
	}

	free(arr);

	return 0;
}
