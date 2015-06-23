
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

string count_and_say(unsigned n) {
	if (n == 1) {
		return "1";
	}

	string prev = count_and_say(n-1);
	ostringstream res;

	string::size_type i = 0;
	while (i < prev.size()) {
		string::size_type count = 0;
		while (i+count < prev.size() && prev[i] == prev[i+count]) {
			count++;
		}
		res << count << prev[i];
		i += count;
	}

	return res.str();
}

int main(void) {
	cout << "Enter a value > 0 to get the n-th element of the Count and Say sequence." << endl;
	cout << "> ";

	unsigned n;
	while (cin >> n) {
		cout << count_and_say(n) << endl;
	}

	return 0;
}
