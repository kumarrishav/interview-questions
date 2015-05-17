
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

static void switch_op(bool arr[], size_t arr_sz, size_t i) {
	if (i > 0) {
		arr[i-1] = !arr[i-1];
	}
	arr[i] = !arr[i];
	if (i < arr_sz-1) {
		arr[i+1] = !arr[i+1];
	}
}

static bool switch_aux(bool source[], bool target[], size_t n, size_t curr, string &buff) {
	if (curr == n) {
		return equal(source, source+n, target);
	}

	if (switch_aux(source, target, n, curr+1, buff)) {
		return true;
	}

	ostringstream to_append;
	to_append << "switch(" << curr << ", source)\n";
	string::size_type size_before = buff.size();
	buff.append(to_append.str());
	switch_op(source, n, curr);

	if (switch_aux(source, target, n, curr+1, buff)) {
		switch_op(source, n, curr); // Leave source unchanged
		return true;
	}

	buff.erase(size_before, buff.size());
	switch_op(source, n, curr);

	return false;
}

bool switch_possible(bool source[], bool target[], size_t n, string &res) {
	return switch_aux(source, target, n, 0, res);
}

int main(void) {
	cout << "Enter size of array, followed by the source array elements (0 / 1), followed by target elements" << endl;
	cout << "> ";

	size_t n;
	while (cin >> n) {
		bool src[n];
		bool target[n];

		for (size_t i = 0; i < n; i++) {
			cin >> src[i];
		}
		for (size_t i = 0; i < n; i++) {
			cin >> target[i];
		}

		string res;
		if (switch_possible(src, target, n, res)) {
			cout << "Found transformation:" << endl;
			cout << res;
		} else {
			cout << "No transformation possible" << endl;
		}

		cout << "> ";
	}

	return 0;
}
