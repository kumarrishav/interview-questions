/* Given an MxN matrix in which each row and each column is sorted in ascending order,
 * write a method to find an element.
 *
 * Source: Cracking the Coding Interview, page 122, question 11.6
 */
#include <stdio.h>

/* Note:
 * The following code implements a more accurate version of bi-dimensional binary search.
 * It does not perform a binary search on each of the sub-matrices diagonals; instead,
 * it picks the middle spot or the sub-matrix (i.e. the "center of mass" of the matrix)
 * and recurses based on that center.
 * In the worst case, the code has to recurse on 3 of the 4 sub-matrices.
 * This solution is O(log(MN)).
 * Although irrelevant for big-O analysis, the logarithm's base is 4/3, since we recurse on
 * 3/4 of the sub-matrices in the worst case.
 *
 * The performance difference between the other solution and this one is big.
 * For a 5000x5000 matrix with 1000 random lookup queries, the other solution
 * took 1 minute and 15 seconds. This solution took 20 seconds -- a considerable improvement
 * on the runtime.
 */

struct cell {
	ssize_t row;
	ssize_t col;
};

static struct cell search_matrix_aux(size_t M, size_t N, int matrix[M][N],
				     ssize_t row_l, ssize_t row_r, ssize_t col_l, ssize_t col_r,
				     int val) {

	struct cell res = { -1, -1 };

	if (row_l > row_r || col_l > col_r) {
		return res;
	}
    
	int mid_x = row_l+(row_r-row_l)/2;
	int mid_y = col_l+(col_r-col_l)/2;
    
	if (matrix[mid_x][mid_y] == val) {
		res.row = mid_x;
		res.col = mid_y;
	} else if (matrix[mid_x][mid_y] < val) {
		res = search_matrix_aux(M, N, matrix, mid_x+1, row_r, col_l, col_r, val);
		if (res.row == -1) {
			res = search_matrix_aux(M, N, matrix, row_l, mid_x, mid_y+1, col_r, val);
		}
	} else {
		res = search_matrix_aux(M, N, matrix, row_l, mid_x-1, mid_y, col_r, val);
		if (res.row == -1) {
			res = search_matrix_aux(M, N, matrix, row_l, row_r, col_l, mid_y-1, val);
		}
	}

	return res;
}

struct cell bin_search_2d(size_t M, size_t N, int matrix[M][N], int val) {
	return search_matrix_aux(M, N, matrix, 0, M-1, 0, N-1, val);
}

int main(void) {
	printf("Enter M, N, matrix elements, and element to find.\n");

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

		int val;
		scanf("%d", &val);

		struct cell res = bin_search_2d(M, N, matrix, val);

		if (res.row == -1) {
			printf("Not found.\n");
		} else {
			printf("Element found at (%zd, %zd)\n", res.row, res.col);
		}
	}

	return 0;
}
