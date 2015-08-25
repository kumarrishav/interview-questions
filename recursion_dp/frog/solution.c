
#include <stdio.h>

int frog_combos(int n) {
	int before_last = 1, last = 0;
	int i;
	for (i = 0; i < n+1; i++) {
		int new_last = before_last + last;
		before_last = last;
		last = new_last;
	}
	return last;
}

int main(void) {
	printf("Enter a value for N.\n");
	printf("> ");

	int n;
	while (scanf("%d", &n) == 1)
		printf("%d\n> ", frog_combos(n));

	return 0;
}
