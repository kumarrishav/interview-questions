/* Given a string, rotate it left by a given amount N.
 *
 * EXAMPLE
 * Input: str = "helloworld", N = 5
 * Output: "worldhello"
 *
 * FOLLOW UP
 *
 * Can you do it in O(n) time and O(1) memory?
 */
#include <stdio.h>
#include <string.h>

void reverse(char *str, size_t begin, size_t end) {
	while (begin < end) {
		end--;
		char tmp = str[begin];
		str[begin] = str[end];
		str[end] = tmp;
		begin++;
	}
}

void left_rotate(char *str, size_t amt) {
	size_t str_sz = strlen(str);
	amt %= str_sz;
	reverse(str, 0, amt);
	reverse(str, amt, str_sz);
	reverse(str, 0, str_sz);
}

static char input_buf[512];
int main(void) {
	printf("Enter a string and a value to rotate it left.\n");
	printf("> ");

	while (scanf("%s", input_buf) == 1) {
		size_t amt;
		scanf("%zu", &amt);
		left_rotate(input_buf, amt);
		printf("%s\n", input_buf);
		printf("> ");
	}

	return 0;
}
