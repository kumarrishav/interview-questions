
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

unsigned anagrams(const vector<string> &strings) {
	set<string> anagrams_seen;
	for (vector<string>::const_iterator str_it = strings.begin(); str_it != strings.end(); str_it++) {
		string key = *str_it;
		sort(key.begin(), key.end());
		anagrams_seen.insert(key);
	}
	return anagrams_seen.size();
}

int main(void) {
	cout << "Enter the size of the list of strings, followed by the strings" << endl;
	cout << "> ";

	vector<string>::size_type list_size;
	while (cin >> list_size) {
		vector<string> strings(list_size);
		for (vector<string>::size_type i = 0; i < list_size; i++) {
			cin >> strings[i];
		}
		cout << "Anagrams: " << anagrams(strings) << endl;
		cout << "> ";
	}

	return 0;
}
