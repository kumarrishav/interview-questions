/* Given a start string, and end string, and a set of words, find if there exists
 * a path from the start string to the end string using words in the set.
 *
 * A path exists if we can get to the end string from the start string by changing
 * only one character at a time. The new string generated after changing one character
 * must be in the words set.
 *
 * EXAMPLE:
 *
 * Words set: bag, cag, cat, fag, con, rat, sat, fog
 * start: cog
 * end: bad
 *
 * One possible path is:
 * cog -> fog -> fag -> bag -> bad
 *
 * Source: Careercup
 */
#include <iostream>
#include <set>
#include <vector>

using namespace std;

bool word_path_aux(set<string> &words, string &start, string &end,
		   vector<string> &path_buff, vector<string> &result) {

	if (start == end) {
		result = path_buff;
		return true;
	}

	if (words.size() == 0) {
		return false;
	}

	for (string::size_type i = 0; i < start.size(); i++) {
		char old = start[i];
		for (char letter = 'a'; letter <= 'z'; letter++) {

			start[i] = letter;
			if (words.find(start) == words.end()) {
				start[i] = old;
				continue;
			}

			words.erase(start);
			path_buff.push_back(start);

			vector<string> res_tmp;
			bool found = word_path_aux(words, start, end, path_buff, res_tmp);

			words.insert(start);
			start[i] = old;

			if (found) {
				result = res_tmp;
				return true;
			}

			path_buff.pop_back();
		}
	}

	return false;
}

bool word_path(set<string> &words, string &start, string &end, vector<string> &result) {
	vector<string> buffer;
	words.insert(end);
	return word_path_aux(words, start, end, buffer, result);
}

int main(void) {
	cout << "Enter size of words set, followed by the words in the set, followed by start and end words" << endl;
	cout << "> ";

	set<string>::size_type words_sz;

	while (cin >> words_sz) {
		set<string> words;
		for (set<string>::size_type i = 0; i < words_sz; i++) {
			string word;
			cin >> word;
			words.insert(word);
		}

		string start, end;
		cin >> start;
		cin >> end;

		cout << endl;

		vector<string> path;
		if (!word_path(words, start, end, path)) {
			cout << "No path found." << endl;
		} else {

			if (path.size() == 0 || path[0] != start) {
				cout << start << endl;
			}

			for (vector<string>::const_iterator it = path.begin(); it != path.end(); it++) {
				cout << *it << endl;
			}

			if (path.size() == 0 || path[path.size()-1] != end) {
				cout << end << endl;
			}

		}

		cout << "> ";
	}

	return 0;
}
