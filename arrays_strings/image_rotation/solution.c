/* Given an image represented by an NxN matrix, where each pixel in the image is
 * 4 bytes, write a method to rotate the image by 90 degrees. Can you do this in
 * place?
 *
 * Source: Cracking the Coding Interview, page 73, question 1.6
 */

#include <stdio.h>

static void swap(size_t N, unsigned matrix[N][N], size_t i1, size_t j1, size_t i2, size_t j2) {
	unsigned tmp = matrix[i1][j1];
	matrix[i1][j1] = matrix[i2][j2];
	matrix[i2][j2] = tmp;
}

static void rotate_layer(size_t N, unsigned matrix[N][N], unsigned l, unsigned width) {
	size_t p;
	for (p = 0; p < width-1; p++) {
		swap(N, matrix, l, l+p, l+p, l+width-1);
		swap(N, matrix, l, l+p, l+width-1, l+width-1-p);
		swap(N, matrix, l, l+p, l+width-1-p, l);
	}
}

void rotate_image(size_t N, unsigned matrix[N][N]) {
	unsigned layers = N/2;

	size_t l;
	for (l = 0; l < layers; l++) {
		rotate_layer(N, matrix, l, N-2*l);
	}
}

static void print_image(size_t N, unsigned matrix[N][N]) {
	size_t i, j;

	for (i = 0; i < N; i++) {
		printf("%u", matrix[i][0]);
		for (j = 1; j < N; j++) {
			printf(" | %u", matrix[i][j]);
		}
		printf("\n");
	}
}

int main(void) {
	printf("Enter N, followed by the NxN values\n");

	size_t dim;
	while (scanf("%zu", &dim) == 1) {
		unsigned image[dim][dim];
		size_t i, j;
		for (i = 0; i < dim; i++) {
			for (j = 0; j < dim; j++) {
				scanf("%u", &image[i][j]);
			}
		}

		printf("Before rotation:\n");
		print_image(dim, image);
		printf("After rotation:\n");
		rotate_image(dim, image);
		print_image(dim, image);
	}

	return 0;
}
