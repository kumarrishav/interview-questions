/* You are given a pattern where a character stands for a word.
 * Design and implement an algorithm that receives a pattern, a string, and finds
 * a word configuration that matches that pattern, if one exists.
 * EXAMPLE:
 * Pattern: "abba"
 * String:  "catdogdogcat"
 *
 * Output: a = cat; b = dog
 *
 * Source: Glassdoor
 */
#include <iostream>
#include <map>

using namespace std;

static bool find_match_aux(const string &str, string::size_type str_offset,
			   const string &vars, string::size_type curr_var,
			   map<char, string> &matches) {
	string::size_type curr_str_sz = str.size()-str_offset;
	string::size_type curr_pattern_sz = vars.size()-curr_var;

	if (curr_str_sz < curr_pattern_sz) {
		return false;
	}

	if (curr_pattern_sz == 0) {
		return curr_str_sz == 0;
	}

	for (string::size_type word_sz = 1; word_sz <= curr_str_sz-curr_pattern_sz+1; word_sz++) {
		map<char, string>::const_iterator match_it = matches.find(vars[curr_var]);
		string curr_word = str.substr(str_offset, word_sz);

		if (match_it != matches.end() && match_it->second != curr_word) {
			continue;
		}

		bool found = false;
		if (match_it == matches.end()) {
			matches[vars[curr_var]] = curr_word;
			found = find_match_aux(str, str_offset+word_sz, vars, curr_var+1, matches);
			if (found) {
				return true;
			}
			matches.erase(vars[curr_var]);
		} else if (find_match_aux(str, str_offset+word_sz, vars, curr_var+1, matches)) {
			return true;
		}
	}

	return false;
}

bool find_match(const string &str, const string &pattern, map<char, string> &matches) {
	return find_match_aux(str, 0, pattern, 0, matches);
}

int main(void) {
	cout << "Enter a pattern followed by a string" << endl;
	cout << "> ";

	string patt, str;
	while (cin >> patt >> str) {
		map<char, string> matches;
		if (!find_match(str, patt, matches)) {
			cout << "Pattern " << patt << " does not match " << str << endl;
		} else {
			for (map<char, string>::const_iterator match_it = matches.begin();
			     match_it != matches.end();
			     match_it++) {
				cout << match_it->first << " -> " << match_it->second << endl;
			}
		}
		cout << "> ";
	}

	return 0;
}
