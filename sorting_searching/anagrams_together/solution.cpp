/* Write a method to sort an array of strings so that all the anagrams are next to
 * each other.
 *
 * Source: Cracking the Coding Interview, page 121, question 11.2
 */
#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
using namespace std;

void sort_by_anagrams(vector<string> &strings) {
	map<string, vector<string> > anagrams_grouped;

	for (vector<string>::const_iterator it = strings.begin(); it != strings.end(); it++) {
		string sorted = *it;
		sort(sorted.begin(), sorted.end());
		anagrams_grouped[sorted].push_back(*it);
	}

	strings.clear();

	for (map<string, vector<string> >::const_iterator it = anagrams_grouped.begin();
	     it != anagrams_grouped.end();
	     it++) {
		for (vector<string>::const_iterator str = it->second.begin();
		     str != it->second.end();
		     str++) {
			strings.push_back(*str);
		}
	}
}

int main(void) {
	cout << "Enter the number of strings, followed by each string." << endl;

	vector<string> strings;
	unsigned str_count;

	while (cin >> str_count) {
		strings.clear();
		for (unsigned i = 0; i < str_count; i++) {
			string str;
			cin >> str;
			strings.push_back(str);
		}

		sort_by_anagrams(strings);

		cout << "Result:" << endl;
		for (vector<string>::const_iterator it = strings.begin();
		     it != strings.end();
		     it++) {
			cout << *it << " ";
		}
		cout << endl;
	}

	return 0;
}
