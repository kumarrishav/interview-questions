/* FOLLOW UP
 * Imagine certain spots are "off limits", such that the robot cannot step on them.
 * Design an algorithm to find a path for the robot from the top left to the bottom right.
 *
 * Source: Cracking the Coding Interview, page 109, question 9.2
 */
#include <stdio.h>
#include <string.h>

int find_path_aux(size_t curr_x, size_t curr_y, size_t x, size_t y,
		  unsigned char bad_pos[x][y], char *path, size_t path_i) {

	if (curr_x == x-1 && curr_y == y-1) {
		path[path_i] = '\0';
		return 1;
	}

	int found_path = 0;

	if (curr_x < x-1 && !bad_pos[curr_x+1][curr_y]) {
		int written = sprintf(path+path_i, " -> (%zu, %zu)", curr_x+1, curr_y);
		found_path = find_path_aux(curr_x+1, curr_y, x, y,
					   bad_pos, path, path_i+written);
	}

	if (!found_path && curr_y < y-1 && !bad_pos[curr_x][curr_y+1]) {
		int written = sprintf(path+path_i, " -> (%zu, %zu)", curr_x, curr_y+1);
		found_path = find_path_aux(curr_x, curr_y+1, x, y,
					   bad_pos, path, path_i+written);
	}

	return found_path;
}

int find_path(size_t x, size_t y, unsigned char bad_pos[x][y], char *path_buff) {
	int written = sprintf(path_buff, "(0, 0)");
	return find_path_aux(0, 0, x, y, bad_pos, path_buff, written);
}

static char path_found[256];

int main(void) {
	printf("Enter the grid dimensions, followed by the number of bad positions,"
	       " followed by the coordinates of each bad position\n");

	size_t x, y;

	while(scanf("%zu%zu", &x, &y) == 2) {

		unsigned char bad_pos[x][y];
		memset(bad_pos, 0, sizeof(bad_pos));

		size_t bad_sz;
		scanf("%zu", &bad_sz);

		size_t i;
		for (i = 0; i < bad_sz; i++) {
			size_t bad_x, bad_y;
			scanf("%zu%zu", &bad_x, &bad_y);
			bad_pos[bad_x][bad_y] = 1;
		}

		if (find_path(x, y, bad_pos, path_found)) {
			printf("Found path: %s\n", path_found);
		} else {
			printf("There's no path.\n");
		}
	}

	return 0;
}
