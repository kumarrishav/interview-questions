
#include <stdio.h>
#include <string.h>
#include <limits.h>

static void swap(char *arr, size_t i, size_t j) {
	char tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}

static void str_permutations_aux(char *str, size_t str_sz, size_t str_i) {
	if (str_i == str_sz) {
		printf("%s\n", str);
		return;
	}

	unsigned char used[UCHAR_MAX];
	memset(used, 0, sizeof(used));

	size_t i;
	for (i = str_i; i < str_sz; i++) {
		if (used[(unsigned char) str[i]]) {
			continue;
		}

		used[(unsigned char) str[i]] = 1;
		swap(str, str_i, i);
		str_permutations_aux(str, str_sz, str_i+1);
		swap(str, str_i, i);
	}
}

void str_permutations(char *str) {
	str_permutations_aux(str, strlen(str), 0);
}

static char input[256];

int main(void) {
	printf("Enter a string to generate all permutations.\n");

	while (scanf("%s", input) == 1) {
		str_permutations(input);
	}

	return 0;
}
