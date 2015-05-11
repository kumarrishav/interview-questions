
#include <stdio.h>

void wildcards_aux(char *str, size_t str_i) {
	if (str[str_i] == '\0') {
		printf("%s\n", str);
		return;
	}

	if (str[str_i] != '?') {
		wildcards_aux(str, str_i+1);
	} else {
		str[str_i] = '0';
		wildcards_aux(str, str_i);
		str[str_i] = '1';
		wildcards_aux(str, str_i);
		str[str_i] = '?';
	}
}

void wildcards(char *str) {
	wildcards_aux(str, 0);
}

static char input_str[512];
int main(void) {
	printf("Enter a string of 0s, 1s and ? to generate all possible combinations\n");
	printf("> ");

	while (scanf("%s", input_str) == 1) {
		wildcards(input_str);
		printf("> ");
	}

	return 0;
}
