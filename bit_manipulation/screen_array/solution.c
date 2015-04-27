/* A monochrome screen is stored as a single array of bytes, allowing eight
 * consecutive pixels to be stored in one byte. The screen has width w, where w is divisible
 * by 8 (that is, no byte will be split across rows). The height of the screen, of course,
 * can be derived from the length of the array and the width. Implement a function
 * drawHorizontalLine(byte[] screen, int width, int x1, int x2, int y) which draws a horizontal
 * line from (x1, y) to (x2, y).
 *
 * Source: Cracking the Coding Interview, page 92, question 5.8
 */
#include <stdio.h>
#include <limits.h>

/* width - size of each row in bits
 * x1, x2 - offset inside row (0 indexed)
 * y - row (0 indexed)
 */
void draw_horizontal_line(unsigned char screen[], int width, int x1, int x2, int y) {

	int cols = width/CHAR_BIT;
	int row = y*cols;

	int first_offset = x1%CHAR_BIT;
	int first_byte = x1/CHAR_BIT;
	int first_full_byte = first_byte;
	if (first_offset != 0) {
		first_full_byte++;
	}

	int last_offset = x2%CHAR_BIT;
	int last_byte = x2/CHAR_BIT;
	int last_full_byte = last_byte;
	if (last_offset != CHAR_BIT-1) {
		last_full_byte--;
	}

	// Fill bytes in between
	int i;
	for (i = first_full_byte; i <= last_full_byte; i++) {
		screen[row+i] = UCHAR_MAX;
	}

	if (first_byte != last_byte) {
		for (i = CHAR_BIT-1-first_offset; i >= 0; i--) {
			screen[row+first_byte] |= 1 << i;
		}
		for (i = CHAR_BIT-1-last_offset; i < CHAR_BIT; i++) {
			screen[row+last_byte] |= 1 << i;
		}
	} else {
		int last_shift = CHAR_BIT-1-last_offset;
		for (i = CHAR_BIT-1-first_offset; i >= last_shift; i--) {
			screen[row+first_byte] |= 1 << i;
		}
	}
}

void print_binary(unsigned char n) {
	size_t unsigned_bits = sizeof(unsigned char)*CHAR_BIT;
	unsigned char i;
	for (i = 0; i < unsigned_bits; i++) {
		printf("%c", (n & (1 << (unsigned_bits-i-1))) ? '1' : '0');
	}
}

void print_screen(unsigned char screen[], int screen_length, int width) {
	int rows = (screen_length*CHAR_BIT)/width;
	int cols = width/CHAR_BIT;
	int i;
	for (i = 0; i < rows; i++) {
		int j;
		if (cols > 0) {
			print_binary(screen[i*cols]);
		}
		for (j = 1; j < cols; j++) {
			printf(" | ");
			print_binary(screen[i*cols+j]);
		}
		printf("\n");
	}
}

// A 4x8 screen
static unsigned char pixels_array[8*4];
static int pixels_array_width = 8*CHAR_BIT;

int main(void) {
	printf("Enter x1, x2, y\n");
	int x1, x2, y;

	while (scanf("%d%d%d", &x1, &x2, &y) == 3) {
		printf("BEFORE:\n");
		print_screen(pixels_array, sizeof(pixels_array), pixels_array_width);
		draw_horizontal_line(pixels_array, pixels_array_width, x1, x2, y);
		printf("AFTER:\n");
		print_screen(pixels_array, sizeof(pixels_array), pixels_array_width);
	}

	return 0;
}
