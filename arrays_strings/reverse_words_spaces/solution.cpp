
#include <iostream>
#include <sstream>
#include <algorithm>

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
		//cout << "swapping word [" << w1_start << ", " << w1_len << "] with [" << w2_start << ", " << w2_len << "]" << endl;
		swap_words(str, w1_start, w1_len, w2_start, w2_len);
		//cout << "done: " << str << endl;

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
