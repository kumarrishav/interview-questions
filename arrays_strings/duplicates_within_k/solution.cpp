/* Given an array of N integers and a value K, design and implement an
 * algorithm that checks if there are duplicates within K indices in the array
 *
 * Source: Careercup (Palantir interview)
 */
#include <iostream>
#include <set>
#include <vector>

using namespace std;

bool duplicates_within_k(const vector<int> &array, vector<int>::size_type k) {
	if (k+1 > array.size()) {
		return false;
	}

	set<int> window;

	for (vector<int>::size_type i = 0; i < k+1; i++) {
		if (!window.insert(array[i]).second) {
			return true;
		}
	}

	for (vector<int>::size_type i = k+1; i < array.size(); i++) {
		window.erase(array[i-k-1]);
		if (!window.insert(array[i]).second) {
			return true;
		}
	}

	return false;
}

int main(void) {
	cout << "Enter the array size, followed by K, followed by the array elements" << endl;
	cout << "> ";

	vector<int>::size_type array_sz;
	vector<int>::size_type k;
	while (cin >> array_sz >> k) {
		vector<int> array;
		for (vector<int>::size_type i = 0; i < array_sz; i++) {
			int val;
			cin >> val;
			array.push_back(val);
		}

		if (duplicates_within_k(array, k)) {
			cout << "true" << endl;
		} else {
			cout << "false" << endl;
		}

		cout << "> ";
	}

	return 0;
}
