/* Given an MxN matrix in which each row and each column is sorted in ascending order,
 * write a method to find an element.
 *
 * Source: Cracking the Coding Interview, page 122, question 11.6
 */
#include <stdio.h>

/* Note:
 * The following code implements the solution outlined in the book.
 * It makes binary search on the diagonal of the matrix, partitions the matrix,
 * and recurses appropriately.
 * Because of the binary search in the diagonal, the running time
 * is roughly O(log(MN)*log(min(M, N))
 */

struct cell {
	ssize_t row;
	ssize_t col;
};

ssize_t bin_search_diagonal(size_t M, size_t N, int matrix[M][N], ssize_t row_l, ssize_t row_r,
			    ssize_t col_l, ssize_t col_r, int val) {
	ssize_t x_l = row_l;
	ssize_t y_l = col_l;

	ssize_t x_r = row_l+col_r-col_l;
	ssize_t y_r = col_r;

	if (x_r > row_r || y_r > col_r) {
		return 0;
	}

	while (x_l <= x_r && y_l <= y_r) {
		ssize_t mid_x = x_l+(x_r-x_l)/2;
		ssize_t mid_y = y_l+(y_r-y_l)/2;

		if (matrix[mid_x][mid_y] == val) {
			return mid_x-row_l;
		} else if (matrix[mid_x][mid_y] < val) {
			x_l = mid_x+1;
			y_l = mid_y+1;
		} else {
			x_r = mid_x-1;
			y_r = mid_y-1;
		}
	}

	if (x_r < row_l) {
		return 0;
	} else if (x_l > row_r) {
		return row_r-row_l;
	} else {
		return x_r-row_l;
	}
}

struct cell bin_search_2d_aux(size_t M, size_t N, int matrix[M][N],
			  ssize_t row_l, ssize_t row_r, ssize_t col_l, ssize_t col_r,
			  int val) {
	struct cell res = { -1, -1 };
	if (row_l > row_r || col_l > col_r) {
		return res;
	}

	if (row_l == row_r && col_l == col_r) {
		if (matrix[row_l][col_l] == val) {
			res.row = row_l;
			res.col = col_l;
		}
		return res;
	}

	ssize_t diag_l = bin_search_diagonal(M, N, matrix, row_l, row_r, col_l, col_r, val);
	ssize_t diag_h = diag_l+1;
	ssize_t diag_row_l = row_l+diag_l;
	ssize_t diag_col_l = col_l+diag_l;
	ssize_t diag_row_h = row_l+diag_h;
	ssize_t diag_col_h = col_l+diag_h;

	if (matrix[diag_row_l][diag_col_l] == val) {
		res.row = diag_row_l;
		res.col = diag_col_l;
		return res;
	} else if (val < matrix[diag_row_l][diag_col_l]) {
		res = bin_search_2d_aux(M, N, matrix, row_l, diag_row_l, col_l, diag_col_l, val);
	} else if (diag_row_h <= row_r && diag_col_h <= col_r &&
		   val > matrix[diag_row_h][diag_col_h]) {
		res = bin_search_2d_aux(M, N, matrix, diag_row_h, row_r, diag_col_h, col_r, val);
	}

	if (res.row == -1) {
		res = bin_search_2d_aux(M, N, matrix, row_l, diag_row_l, diag_col_h, col_r, val);
	}
	if (res.row == -1) {
		res = bin_search_2d_aux(M, N, matrix, diag_row_h, row_r, col_l, diag_col_l, val);
	}

	return res;
}

struct cell bin_search_2d(size_t M, size_t N, int matrix[M][N], int val) {
	return bin_search_2d_aux(M, N, matrix, 0, M-1, 0, N-1, val);
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
