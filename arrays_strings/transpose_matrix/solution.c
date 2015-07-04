/* Write code to transpose a matrix inplace */

#include <stdio.h>

void transpose(size_t n, int matrix[n][n]) {
	size_t i, j;
	for (i = 0; i < n-1; i++) {
		for (j = i+1; j < n; j++) {
			int tmp = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = tmp;
		}
	}
}

void print_matrix(size_t n, int matrix[n][n]) {
	size_t i, j;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(void) {
	printf("Enter dimension N of matrix, followed by the NxN elements\n");
	printf("> ");

	size_t n;
	while (scanf("%zu", &n) == 1) {
		int matrix[n][n];

		size_t i, j;
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				scanf("%d", &matrix[i][j]);
			}
		}

		transpose(n, matrix);
		print_matrix(n, matrix);

		printf("> ");
	}

	return 0;
}
