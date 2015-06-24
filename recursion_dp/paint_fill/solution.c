/* Implement the "paint fill" function that one might see on many image editing
 * programs. That is, given a screen (represented by a two-dimensional array of
 * colors), a point, and a new color, fill in the surrounding area until the color
 * changes from the original color.
 *
 * Source: Cracking the Coding Interview, page 110, question 9.7
 */
#include <stdio.h>

static void paint_fill_aux(size_t x, size_t y, char screen[x][y], size_t curr_x, size_t curr_y,
		    char original_color, char new_color) {

	if (screen[curr_x][curr_y] != original_color) {
		return;
	}

	screen[curr_x][curr_y] = new_color;

	if (curr_x > 0) {
		paint_fill_aux(x, y, screen, curr_x-1, curr_y, original_color, new_color);
	}
	if (curr_x < x-1) {
		paint_fill_aux(x, y, screen, curr_x+1, curr_y, original_color, new_color);
	}
	if (curr_y > 0) {
		paint_fill_aux(x, y, screen, curr_x, curr_y-1, original_color, new_color);
	}
	if (curr_y < y-1) {
		paint_fill_aux(x, y, screen, curr_x, curr_y+1, original_color, new_color);
	}
}

void paint_fill(size_t x, size_t y, char screen[x][y], size_t px, size_t py, char new_color) {
	// Important: if screen[px][py] == new_color, we would have infinite recursion
	if (screen[px][py] != new_color) {
		paint_fill_aux(x, y, screen, px, py, screen[px][py], new_color);
	}
}

void print_screen(size_t x, size_t y, char screen[x][y]) {
	size_t i;
	for (i = 0; i < x; i++) {
		size_t j;
		for (j = 0; j < y; j++) {
			putchar(screen[i][j]);
		}
		putchar('\n');
	}
}

int main(void) {
	printf("Enter screen size (X x Y), an each cell's color.\n");

	size_t x, y;
	scanf("%zu%zu", &x, &y);
	char screen[x][y];

	size_t i;
	for (i = 0; i < x; i++) {
		size_t j;
		for (j = 0; j < y; j++) {
			scanf(" %c", &screen[i][j]);
		}
	}

	printf("Now enter your queries. The query format is: X Y C, "
	       "where X and Y indicate the target cell, and C the new color.\n");

	size_t px, py;
	char new_color;
	while (scanf("%zu%zu %c", &px, &py, &new_color) == 3) {
		printf("Before:\n");
		print_screen(x, y, screen);
		printf("After:\n");
		paint_fill(x, y, screen, px, py, new_color);
		print_screen(x, y, screen);
	}

	return 0;
}
