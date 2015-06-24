/* You are given two boolean arrays, A and B, of the same size.
 *
 * There is an auxiliary function, switch_op(bool *, size_t, size_t), which
 * receives an array, its size N, and an index i, and flips the boolean value
 * in position i-1 (if i > 0), i, and i+1 (if i < N-1).
 *
 * For example, for the array [true, false, false, true], switch_op() on 0 will turn it into
 * [false, true, false, true]. switch_op() on 1 will turn it into [false, true, true, true],
 * and switch_op() on 3 turns it into [true, false, true, false].
 *
 * Write a function that finds the necessary steps to transform A into B using only switch_op().
 *
 * Source: StackOverflow (http://stackoverflow.com/questions/7292118/interview-question-recursion-backtracking)
 */
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
