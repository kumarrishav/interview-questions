/* Write an algorithm to print all ways of arranging eight queens on an 8x8 chess
 * board so that none of them share the same row, column or diagonal. In this case,
 * "diagonal" means all diagonals, not just the two that bisect the board.
 *
 * Source: Cracking the Coding Interview, page 110, question 9.9
 */
#include <stdio.h>
#include <string.h>

static void update_queen(size_t x, unsigned board[x][x], int px, int py, int val) {
	board[px][py] += val;

	int i;
	for (i = px-1; i >= 0; i--) {
		board[i][py] += val;
	}
	for (i = px+1; i < x; i++) {
		board[i][py] += val;
	}
	for (i = py-1; i >= 0; i--) {
		board[px][i] += val;
	}
	for (i = py+1; i < x; i++) {
		board[px][i] += val;
	}

	int dx, dy;
	for (dx = px-1, dy = py-1; dx >= 0 && dy >= 0; dx--, dy--) {
		board[dx][dy] += val;
	}
	for (dx = px-1, dy = py+1; dx >= 0 && dy < x; dx--, dy++) {
		board[dx][dy] += val;
	}
	for (dx = px+1, dy = py-1; dx < x && dy >= 0; dx++, dy--) {
		board[dx][dy] += val;
	}
	for (dx = px+1, dy = py+1; dx < x && dy < x; dx++, dy++) {
		board[dx][dy] += val;
	}
}

static void place_queen(size_t x, unsigned board[x][x], size_t px, size_t py) {
	update_queen(x, board, px, py, 1);
}

static void remove_queen(size_t x, unsigned board[x][x], size_t px, size_t py) {
	update_queen(x, board, px, py, -1);
}

unsigned long long queens_arrangements_aux(size_t x, unsigned board[x][x], size_t curr_row) {
	if (curr_row == x) {
		return 1;
	}
	unsigned long long res = 0;
	size_t i;
	for (i = 0; i < x; i++) {
		if (board[curr_row][i] == 0) {
			place_queen(x, board, curr_row, i);
			res += queens_arrangements_aux(x, board, curr_row+1);
			remove_queen(x, board, curr_row, i);
		}
	}

	return res;
}

unsigned long long queens_arrangements(size_t x) {
	unsigned board[x][x];
	memset(board, 0, sizeof(board));
	return queens_arrangements_aux(x, board, 0);
}

/* A couple of interesting remarks:
 * - There is a solution for every N except N = 2 and N = 3
 * - The traditional 8-queens problem has 92 solutions, which may seem like a lot at first
 */
int main(void) {
	printf("Enter board size N (board is NxN)\n");
	printf("> ");
	fflush(stdout);

	size_t n;
	while (scanf("%zu", &n) == 1) {
		printf("There are %llu ways to arrange the queens.\n", queens_arrangements(n));
		printf("> ");
	}

	return 0;
}
