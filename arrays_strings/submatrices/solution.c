/* Given a matrix of N rows by M columns, write a program to find and print
 * every submatrix of that matrix.
 *
 * EXAMPLE
 *
 * Consider the 2x2 matrix:
 *
 *  --- ---
 * | 1 | 2 |
 *  --- ---
 * | 3 | 4 |
 *  --- ---
 *
 * The submatrices are:
 *
 *  ---
 * | 1 |
 *  ---
 *
 *  ---
 * | 2 |
 *  ---
 *
 *  ---
 * | 3 |
 *  ---
 *
 *  ---
 * | 4 |
 *  ---
 *
 *  --- ---
 * | 1 | 2 |
 *  --- ---
 *
 *  --- ---
 * | 3 | 4 |
 *  --- ---
 *
 *  ---
 * | 1 |
 *  ---
 * | 3 |
 *  ---
 *
 *  ---
 * | 2 |
 *  ---
 * | 4 |
 *  ---
 *
 *  --- ---
 * | 1 | 2 |
 *  --- ---
 * | 3 | 4 |
 *  --- ---
 *
 * Source: Careercup
 */
#include <stdio.h>

static void print_submatrix(size_t N, size_t M, int matrix[N][M],
			    size_t row_begin, size_t col_begin,
			    size_t rows, size_t cols) {
	size_t i, j;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			printf("%d ", matrix[row_begin+i][col_begin+j]);
		}
		putchar('\n');
	}

	putchar('\n');
}

static void submatrices(size_t N, size_t M, int matrix[N][M], size_t rows, size_t cols) {
	size_t i, j;
	for (i = 0; i+rows-1 < N; i++) {
		for (j = 0; j+cols-1 < M; j++) {
			print_submatrix(N, M, matrix, i, j, rows, cols);
		}
	}
}

void all_submatrices(size_t N, size_t M, int matrix[N][M]) {
	size_t i, j;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= M; j++) {
			submatrices(N, M, matrix, i, j);
		}
	}
}

int main(void) {
	printf("Enter the matrix dimensions, followed by the elements.\n");
	printf("> ");

	size_t N, M;
	while (scanf("%zu%zu", &N, &M) == 2) {
		int matrix[N][M];

		size_t i, j;
		for (i = 0; i < N; i++) {
			for (j = 0; j < M; j++) {
				scanf("%d", &matrix[i][j]);
			}
		}

		all_submatrices(N, M, matrix);
		printf("> ");
	}

	return 0;
}
