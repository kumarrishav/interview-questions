
#include <stdio.h>
#define abs(x) ((x) > 0 ? (x) : -(x))

int recursive_multiply(int a, int b) {
	if (a == 0 || b == 0)
		return 0;
	if (a == 1 || b == 1)
		return a == 1 ? b : a;
	if ((a < 0) != (b < 0))
		return -recursive_multiply(abs(a), abs(b));

	a = abs(a);
	b = abs(b);

	int tmp = recursive_multiply(a >> 1, b);
	tmp *= 2;
	if (a & 1)
		tmp += b;

	return tmp;
}

int main(void) {
	printf("Enter A and B to multiply\n");
	printf("> ");

	int a, b;
	while (scanf("%d%d", &a, &b) == 2) {
		printf("%d x %d = %d\n> ", a, b, recursive_multiply(a, b));
	}

	return 0;
}
