#include <stdio.h>
#include <string.h>

static void swap(char *str, size_t i, size_t j) {
	char tmp = str[i];
	str[i] = str[j];
	str[j] = tmp;
}

char *reverse_str(char *str) {
	if (str == NULL) {
		return NULL;
	}

	if (*str == '\0') {
		return str;
	}

	size_t l, r;
	for (l = 0, r = strlen(str)-1; l < r; l++, r--) {
		swap(str, l, r);
	}

	return str;
}

static char buff[1024];
int main(void) {
	printf("Enter a string to reverse\n");

	while (scanf("%s", buff) == 1) {
		printf("Received:\t%s\n", buff);
		printf("Reversed:\t%s\n", reverse_str(buff));
	}

	return 0;
}
