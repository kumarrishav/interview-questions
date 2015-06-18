/* A color matrix is represented as a 2D array where each element is one of 'R', 'G', 'B', or '.'.
 * Elements marked with '.' have no color.
 *
 * You are to fill the colorless spots such that the entire matrix is colored.
 * To do so, you have an infinite set of spare stripes. A stripe can be red or blue.
 *
 * If a red stripe is placed in position (i, j), then it will color every colorless position
 * downwards until the end of the matrix or until a colored position is found.
 *
 * A blue stripe is similar, but it expands to the right instead of going down.
 *
 * For example, in this matrix:
 *
 * . . B . .
 * . . G R R
 * . . B . .
 * R . . . .
 * R . . . .
 *
 * Placing a blue stripe in position (0,0) would yield:
 *
 * B B B . .
 * . . G R R
 * . . B . .
 * R . . . .
 * R . . . .
 *
 * Placing a red string in (0, 0) would yield:
 * R . B . .
 * R . G R R
 * R . B . .
 * R . . . .
 * R . . . .
 *
 * Design and implement an algorithm that finds the minimum number of stripes that must be
 * used in order to color the entire matrix
 *
 * Source: Careercup
 */
#include <stdio.h>
#include <limits.h>
#define min(a, b) ((a) < (b) ? (a) : (b))

int place_blue_strip(int rows, int cols, char grid[rows][cols], int row, int col) {
	int i;
	for (i = row; i < rows && grid[i][col] == '.'; i++) {
		grid[i][col] = 'B';
	}
	return i-row;
}

void delete_blue_strip(int rows, int cols, char grid[rows][cols], int row, int col, int filled) {
	int i;
	for (i = row; i < row+filled; i++) {
		grid[i][col] = '.';
	}
}

int place_red_strip(int rows, int cols, char grid[rows][cols], int row, int col) {
	int i;
	for (i = col; i < cols && grid[row][i] == '.'; i++) {
		grid[row][i] = 'R';
	}
	return i-col;
}

void delete_red_strip(int rows, int cols, char grid[rows][cols], int row, int col, int filled) {
	int i;
	for (i = col; i < col+filled; i++) {
		grid[row][i] = '.';
	}
}

int min_strips(int rows, int cols, char grid[rows][cols]) {
	int i;
	for (i = 0; i < rows; i++) {
		int j;
		for (j = 0; j < cols; j++) {
			if (grid[i][j] == '.') {
				int filled;
				int res;

				filled = place_blue_strip(rows, cols, grid, i, j);
				res = min_strips(rows, cols, grid)+1;

				delete_blue_strip(rows, cols, grid, i, j, filled);

				filled = place_red_strip(rows, cols, grid, i, j);
				int res_red = min_strips(rows, cols, grid);
				res = min(res, res_red+1);

				delete_red_strip(rows, cols, grid, i, j, filled);
				return res;
			}
		}
	}
	return 0;
}

int main(void) {
	printf("Enter grid dimensions, followed by each element\n");
	printf("> ");

	int rows, cols;
	while (scanf("%d%d", &rows, &cols) == 2) {
		char grid[rows][cols];
		int i, j;
		for (i = 0; i < rows; i++) {
			for (j = 0; j < cols; j++) {
				scanf(" %c", &grid[i][j]);
			}
		}
		printf("Min strips = %d\n", min_strips(rows, cols, grid));
		printf("> ");
	}

	return 0;
}
