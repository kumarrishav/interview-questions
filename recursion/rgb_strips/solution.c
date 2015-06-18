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

static void print_map(int rows, int cols, char grid[rows][cols]) {
	int i, j;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			putchar(grid[i][j]);
			putchar(' ');
		}
		putchar('\n');
	}
}

int min_strips(int rows, int cols, char grid[rows][cols]) {
	printf("Entered min_strips()\n");
	print_map(rows, cols, grid);

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
				printf("returning %d\n", res);
				return res;
			}
		}
	}
	printf("returning 0\n");
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
