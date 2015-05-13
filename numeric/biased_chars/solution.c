
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

static size_t indexes[100];

void prepare_stream(size_t n, int prob_distribution[n]) {
	int cum_sum = 0;
	size_t last_idx = 0;

	size_t i;
	for (i = 0; i < n; i++) {
		cum_sum += prob_distribution[i];
		assert(cum_sum <= 100);
		while (last_idx != cum_sum) {
			indexes[last_idx++] = i;
		}
	}

	assert(cum_sum == 100);
}

char generate_char(size_t n, char chars_distribution[n]) {
	return chars_distribution[indexes[rand()%100]];
}

int main(void) {
	printf("Enter how many chars you want to generate, the size of P, the values in P (0-100),\n"
	       "and the chars in C\n");
	printf("> ");

	srand(time(NULL));

	size_t to_generate;
	size_t n;
	while (scanf("%zu%zu", &to_generate, &n) == 2) {
		int distr[n];
		char chars_distr[n];

		size_t i;
		for (i = 0; i < n; i++) {
			scanf("%d", &distr[i]);
		}
		for (i = 0; i < n; i++) {
			scanf(" %c", &chars_distr[i]);
		}

		prepare_stream(n, distr);

		for (i = 0; i < to_generate; i++) {
			putchar(generate_char(n, chars_distr));
			putchar('\n');
		}

		printf("> ");
	}

	return 0;
}
