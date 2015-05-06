/* Given an MxN matrix in which each row and each column is sorted in ascending order,
 * write a method to find an element.
 *
 * Source: Cracking the Coding Interview, page 122, question 11.6
 */
#include <stdio.h>

struct cell {
	ssize_t row;
	ssize_t col;
};

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

	ssize_t diag_x = row_l+(row_r-row_l)/2;
	ssize_t diag_y = col_l+(col_r-col_l)/2;

	if (matrix[diag_x][diag_y] == val) {
		res.row = diag_x;
		res.col = diag_y;
		return res;
	} else if (val < matrix[diag_x][diag_y]) {
		res = bin_search_2d_aux(M, N, matrix, row_l, diag_x, col_l, diag_y, val);
	} else if (diag_x+1 <= row_r && diag_y+1 <= col_r && matrix[diag_x+1][diag_y+1] == val) {
		res.row = diag_x+1;
		res.col = diag_y+1;
		return res;
	} else if (diag_x+1 <= row_r && diag_y+1 <= col_r && val > matrix[diag_x+1][diag_y+1]) {
		res = bin_search_2d_aux(M, N, matrix, diag_x+1, row_r, diag_y+1, col_r, val);
	}


	if (res.row == -1) {
		res = bin_search_2d_aux(M, N, matrix, diag_x+1, row_r, col_l, diag_y, val);
	}
	if (res.row == -1) {
		res = bin_search_2d_aux(M, N, matrix, row_l, diag_x, diag_y+1, col_r, val);
	}

	return res;
}

struct cell bin_search_2d(size_t M, size_t N, int matrix[M][N], int val) {
	return bin_search_2d_aux(M, N, matrix, 0, M-1, 0, N-1, val);
}

#define M 5000
#define N 5000
static int matrix[M][N];

int main(void) {

	size_t i;
	for (i = 0; i < M; i++) {
		size_t j;
		for (j = 0; j < N; j++) {
			scanf("%d", &matrix[i][j]);
		}
	}

	int val;
	while (scanf("%d", &val) == 1) {
		struct cell res = bin_search_2d(M, N, matrix, val);

		if (res.row == -1) {
			printf("Not found.\n");
		} else {
			printf("Element found at (%zd, %zd)\n", res.row, res.col);
		}
	}

	return 0;
}
