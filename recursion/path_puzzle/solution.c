/* Suppose you are given a puzzle that is represented as a matrix with 0s and 1s, where a 0
 * indicates you're allowed to move into that position and 1 means you're not.
 *
 * Write a function that given a start position and an end position, returns a boolean value
 * indicating if there exists a path from start to end. You are only allowed to move up, left,
 * right and down. Diagonal movement is not allowed.
 *
 * Source: Careercup (Amazon interview)
 */
#include <stdio.h>

static int valid_move(int x, int y, int puzzle[x][y], int pos_x, int pos_y) {
	return pos_x >= 0 && pos_x < x && pos_y >= 0 && pos_y < y && puzzle[pos_x][pos_y] == 0;
}

int find_path(int x, int y, int puzzle[x][y], int curr_x, int curr_y, int goal_x, int goal_y) {

	if (goal_x == curr_x && goal_y == curr_y) {
		return 1;
	}

	int saved = puzzle[curr_x][curr_y];
	puzzle[curr_x][curr_y] = 1;

	int done = 0;
	int i;
	for (i = -1; !done && i < 2; i++) {
		int j;
		for (j = -1; !done && j < 2; j++) {
			if ((i == 0) != (j == 0) && valid_move(x, y, puzzle, curr_x+i, curr_y+j)) {
				done = find_path(x, y, puzzle, curr_x+i, curr_y+j, goal_x, goal_y);
			}
		}
	}

	puzzle[curr_x][curr_y] = saved; // Always 0 unless puzzle[orig_x][orig_y] == 1

	return done;
}

int main(void) {
	printf("Enter the dimensions of the puzzle, followed by each puzzle cell element,\n"
	       "followed by the origin coordinates, followed by the target coordinates.\n");
	printf("> ");

	int size_x, size_y;
	while (scanf("%d%d", &size_x, &size_y) == 2) {
		int puzzle[size_x][size_y];

		int i;
		for (i = 0; i < size_x; i++) {
			int j;
			for (j = 0; j < size_y; j++) {
				scanf("%d", &puzzle[i][j]);
			}
		}

		int origin_x, origin_y;
		int target_x, target_y;

		scanf("%d%d%d%d", &origin_x, &origin_y, &target_x, &target_y);

		if (find_path(size_x, size_y, puzzle, origin_x, origin_y, target_x, target_y)) {
			printf("Found path.\n");
		} else {
			printf("Path not found.\n");
		}

		printf("> ");
	}

	return 0;
}
