/* You are given a set top box which contains every letter in the alphabet exactly once,
 * in any order.
 *
 * This is an example of a box:
 *
 * +-------------------+
 * | a | b | c | d | e |
 * | - | - | - | - | - |
 * | f | g | h | i | j |
 * | - | - | - | - | - |
 * | k | l | m | n | o |
 * | - | - | - | - | - |
 * | p | q | r | s | t |
 * | - | - | - | - | - |
 * | u | v | w | x | y |
 * | - | - | - | - | - |
 * | z |   |   |   |   |
 * +-------------------+
 *
 * There is a cursor that always starts on the top left corner position of the box.
 *
 * You are to write a method that prints out the steps necessary to form a given word. These steps
 * are described in the form of a path.
 *
 * Each time the cursor moves left, right, up or down, the respective move is added to the path
 * ("LEFT", "RIGHT", "UP", "DOWN")
 *
 * To select a position in the path as part of the word, add the word "OK" to the path.
 *
 * EXAMPLE:
 *
 * For the above box, we get the word "con" with the following steps (you can find and print any
 * valid path that forms the word; this is just one of them):
 *
 * RIGHT (moves the cursor to 'b')
 * RIGHT (moves the cursor to 'c')
 * OK (selects this position - the word so far is "c")
 * LEFT (moves the cursor to 'b')
 * LEFT (moves the cursor to 'a')
 * DOWN (moves the cursor to 'f')
 * RIGHT (moves the cursor to 'g')
 * RIGHT (moves the cursor to 'h')
 * RIGHT (moves the cursor to 'i')
 * RIGHT (moves the cursor to 'j')
 * DOWN (moves the cursor to 'o')
 * OK (selects this position - the word so far is "co")
 * LEFT (moves the cursor to 'n')
 * OK (selects this position - the word so far is "con")
 *
 * Source: Careercup (Google interview)
 */
#include <stdio.h>
#include <string.h>

static const int top_box_rows = 6;
static const int top_box_dims[] = { 5, 5, 5, 5, 5, 1 };

static int top_box_path(const char *word, size_t word_i, char *out_buff, size_t out_i,
			char box[][5], int curr_x, int curr_y, int visited[][5]) {

	if (curr_x < 0 || curr_x >= top_box_rows || curr_y < 0 || curr_y >= top_box_dims[curr_x] ||
	    visited[curr_x][curr_y] == word_i) {
		return 0;
	}

	visited[curr_x][curr_y] = word_i;

	if (box[curr_x][curr_y] == word[word_i]) {
		out_i += sprintf(&out_buff[out_i], "OK\n");
		word_i++;
	}

	if (word[word_i] == '\0') {
		printf("%s", out_buff);
		return 1;
	}

	size_t out_i_incr;
	out_i_incr = sprintf(&out_buff[out_i], "LEFT\n");
	if (top_box_path(word, word_i, out_buff, out_i+out_i_incr, box, curr_x, curr_y-1, visited)) {
		return 1;
	}

	out_i_incr = sprintf(&out_buff[out_i], "RIGHT\n");
	if (top_box_path(word, word_i, out_buff, out_i+out_i_incr, box, curr_x, curr_y+1, visited)) {
		return 1;
	}

	out_i_incr = sprintf(&out_buff[out_i], "UP\n");
	if (top_box_path(word, word_i, out_buff, out_i+out_i_incr, box, curr_x-1, curr_y, visited)) {
		return 1;
	}

	out_i_incr = sprintf(&out_buff[out_i], "DOWN\n");
	if (top_box_path(word, word_i, out_buff, out_i+out_i_incr, box, curr_x+1, curr_y, visited)) {
		return 1;
	}

	return 0;
}

void top_box(const char *word, char box[][5]) {
	static char buff[1024];
	static int visited_buff[6][5];
	memset(visited_buff, -1, sizeof(visited_buff));
	(void) top_box_path(word, 0, buff, 0, box, 0, 0, visited_buff);
}

static char box[6][5];
static char word[512];
int main(void) {
	printf("Enter a box, followed by a word\n");
	printf("> ");

	while (1) {
		size_t i;

		for (i = 0; i < top_box_rows; i++) {
			size_t j;
			for (j = 0; j < top_box_dims[i]; j++) {
				scanf(" %c", &box[i][j]);
			}
		}

		if (scanf("%s", word) != 1) {
			return 0;
		}

		top_box(word, box);

		printf("> ");
	}

	return 0;
}
