
#include <stdio.h>

/* Recursive approach */
unsigned long long paths_aux(size_t curr_x, size_t curr_y, size_t x, size_t y) {
	unsigned long long res = 0;

	if (curr_x == x && curr_y == y) {
		return 1;
	}

	if (curr_x < x) {
		res += paths_aux(curr_x+1, curr_y, x, y);
	}

	if (curr_y < y) {
		res += paths_aux(curr_x, curr_y+1, x, y);
	}

	return res;
}

unsigned long long paths_rec(size_t x, size_t y) {
	return paths_aux(0, 0, x, y);
}

/* Maths approach */
unsigned long long factorial(unsigned n);
unsigned long long binomial(unsigned n, unsigned r);
unsigned long long paths_math(size_t x, size_t y) {
	return binomial(x+y, x);
}

unsigned long long factorial(unsigned n) {
	unsigned long long res = 1;

	while (n > 1) {
		res *= n;
		n--;
	}

	return res;
}

unsigned long long binomial(unsigned n, unsigned r) {
	return factorial(n)/(factorial(r)*factorial(n-r));
}

int main(void) {
	size_t x, y;

	printf("Enter the values for X and Y\n");

	while (scanf("%zu%zu", &x, &y) == 2) {

		unsigned long long rec = paths_rec(x, y);
		unsigned long long maths = paths_math(x, y);

		printf("Recursive: %llu\n", rec);
		printf("Maths: %llu\n", maths);
	}

	return 0;
}
