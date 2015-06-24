/* Implement a function that prints all possible combinations of the
 * characters in a string. These combinations range in length from one to the length
 * of the string. Two combinations that differ only in ordering of their characters are
 * the same combination. In other words, "12" and "31" are different combinations
 * from the input string "123", but "21" is the same as "12".
 */
#include <stdio.h>
#include <string.h>

/* Note:
 * This solution assumes there are no duplicate characters in the string.
 * This is definitely something worth asking the interviewer before
 * designing the algorithm.
 */

static void print_combinations_aux(const char *str, size_t str_sz, size_t str_i,
			    char *buff, size_t buff_i) {
	if (str_i == str_sz) {
		buff[buff_i] = '\0';
		// We do not want the empty subset
		if (buff_i != 0) {
			printf("%s\n", buff);
		}
		return;
	}

	print_combinations_aux(str, str_sz, str_i+1, buff, buff_i);
	buff[buff_i] = str[str_i];
	print_combinations_aux(str, str_sz, str_i+1, buff, buff_i+1);

}

void print_combinations(const char *str) {
	size_t str_sz = strlen(str);
	char buff[str_sz+1];
	print_combinations_aux(str, str_sz, 0, buff, 0);
}

static char input_buff[1024];
int main(void) {
	printf("Enter a string to get its powerset.\n");
	printf("> ");

	while (scanf("%s", input_buff) == 1) {
		print_combinations(input_buff);
		printf("> ");
	}

	return 0;
}
