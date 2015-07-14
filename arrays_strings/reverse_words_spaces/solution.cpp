/* Reverse the words in a string, but maintain whitespace distribution. That is, you are to reverse
 * the words in a string, but keep the number of whitespaces separating each word.
 *
 * EXAMPLE
 * In the following example, consider that whitespaces are represented by S.
 *
 * Input: SShelloSSSSeveryoneSinSSSSStheSroomSSSSS
 * Output: SSroomSSSStheSinSSSSSeveryoneShelloSSSSS
 *
 * Source: Invented
 */

#include <iostream>
#include <sstream>
#include <algorithm>

/* We show two approaches below: one runs in O(N^2) time but uses O(1) memory.
 * The other runs in O(N) time but uses O(N) memory.
 *
 * The former is based out of the swap_words() operation, which swaps the first word of a substring
 * with the last word of that substring.
 * As can be seen, this can be performed with a bunch of string rotations if we know the length of
 * each word and its starting index. String rotations are implemented with reverse operations
 * as described in the classical algorithms textbook Programing Pearls.
 *
 * The O(N) memory solution is pretty straightforward: we just copy spaces to a new string, and
 * when a word is seen, instead of copying it, we grab the next word from the rear end.
 *
 */

using namespace std;

void rotate_left(string &str, string::size_type start, string::size_type len, string::size_type amt) {
	reverse(str.begin()+start, str.begin()+start+amt);
	reverse(str.begin()+start+amt, str.begin()+start+len);
	reverse(str.begin()+start, str.begin()+start+len);
}

void rotate_right(string &str, string::size_type start, string::size_type len, string::size_type amt) {
	rotate_left(str, start, len, len-amt);
}

void swap_words(string &str, string::size_type w1_start, string::size_type w1_len,
		string::size_type w2_start, string::size_type w2_len) {
	rotate_right(str, w1_start, w2_start+w2_len-w1_start, w2_len);
	rotate_left(str, w1_start, w1_len+w2_len, w2_len);
	rotate_left(str, w1_start, w2_start+w2_len-w1_start, w1_len);
}

string::size_type next_word_forward(const string &str, string::size_type curr_start, string::size_type &olen) {
	olen = 1;
	while (curr_start < str.size() && str[curr_start] == ' ')
		curr_start++;
	while (curr_start+olen < str.size() && str[curr_start+olen] != ' ')
		olen++;
	return curr_start;
}

string::size_type next_word_backward(const string &str, string::size_type curr_start, string::size_type &olen) {
	olen = 0;
	while (curr_start > 0 && str[curr_start] == ' ')
		curr_start--;
	while (curr_start > 0 && str[curr_start] != ' ') {
		curr_start--;
		olen++;
	}

	if (curr_start == 0 && str[curr_start] != ' ') {
		olen++;
		return curr_start;
	}

	return curr_start+1;
}

void reverse_words_nomem(string &str) {
	string::size_type w1_len;
	string::size_type w1_start = next_word_forward(str, 0, w1_len);

	string::size_type w2_len;
	string::size_type w2_start = next_word_backward(str, str.size()-1, w2_len);

	while (w1_start+w1_len-1 < w2_start) {
		swap_words(str, w1_start, w1_len, w2_start, w2_len);
		string::size_type w1_len_tmp = w1_len;
		w1_start = next_word_forward(str, w1_start+w2_len, w1_len);
		w2_start = next_word_backward(str, w2_start+w2_len-w1_len_tmp-1, w2_len);
	}
}

string reverse_words_buffered(const string &str) {
	ostringstream oss;

	string::size_type next_word_len;
	string::size_type next_word_start = str.size();

	string::size_type i = 0;
	while (i < str.size()) {
		if (str[i] == ' ') {
			oss << ' ';
			i++;
			continue;
		}

		string::size_type word_len;
		i = next_word_forward(str, i, word_len);
		i += word_len;

		next_word_start = next_word_backward(str, next_word_start-1, next_word_len);
		oss << str.substr(next_word_start, next_word_len);
	}

	return oss.str();
}

int main(void) {
	cout << "Enter a string to reverse the words order ignoring spaces" << endl;
	cout << "> ";

	string line;
	while (getline(cin, line)) {
		cout << reverse_words_buffered(line) << endl;
		reverse_words_nomem(line);
		cout << line << endl << "> ";
	}

	return 0;
}
