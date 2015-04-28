/* Write an algorithm such that if an element in an MxN matrix is 0, its entire row
 * and column are set to 0.
 *
 * Source: Cracking the Coding Interview, page 73, question 1.7
 */
#include <stdio.h>
#include <string.h>

static void clear_col(size_t M, size_t N, int matrix[M][N], size_t col) {
	size_t i;
	for (i = 0; i < M; i++) {
		matrix[i][col] = 0;
	}
}


static void clear_row(size_t M, size_t N, int matrix[M][N], size_t row) {
	size_t i;
	for (i = 0; i < N; i++) {
		matrix[row][i] = 0;
	}
}

void clear_zero_rows_cols(size_t M, size_t N, int matrix[M][N]) {
	unsigned char rows_to_zero[M];
	unsigned char cols_to_zero[N];

	memset(rows_to_zero, 0, sizeof(rows_to_zero));
	memset(cols_to_zero, 0, sizeof(cols_to_zero));

	size_t i;
	for (i = 0; i < M; i++) {
		size_t j;
		for (j = 0; j < N; j++) {
			if (matrix[i][j] == 0) {
				rows_to_zero[i] = 1;
				cols_to_zero[j] = 1;
			}
		}
	}

	for (i = 0; i < M; i++) {
		if (rows_to_zero[i]) {
			clear_row(M, N, matrix, i);
		}
	}

	for (i = 0; i < N; i++) {
		if (cols_to_zero[i]) {
			clear_col(M, N, matrix, i);
		}
	}
}

static void print_matrix(size_t M, size_t N, int matrix[M][N]) {
	size_t i, j;

	for (i = 0; i < M; i++) {
		printf("%u", matrix[i][0]);
		for (j = 1; j < N; j++) {
			printf(" | %u", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(void) {
	printf("Enter M, N, and MxN elements. Cells with 0 will clear the entire row and rol\n");

	size_t M, N;
	while (scanf("%zu%zu", &M, &N) == 2) {
		int matrix[M][N];

		size_t i;
		for (i = 0; i < M; i++) {
			size_t j;
			for (j = 0; j < N; j++) {
				scanf("%d", &matrix[i][j]);
			}
		}

		printf("Received:\n");
		print_matrix(M, N, matrix);
		printf("Cleared:\n");
		clear_zero_rows_cols(M, N, matrix);
		print_matrix(M, N, matrix);
	}

	return 0;
}
