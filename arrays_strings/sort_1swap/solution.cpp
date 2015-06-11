/* Given an array, determine whether it is possible to sort it by doing one swap only
 *
 * Source: Careercup
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

bool sort_1swap(const vector<int> &arr) {
	vector<int> sorted = arr;
	sort(sorted.begin(), sorted.end());

	vector<int>::size_type diff = 0;
	vector<int>::size_type i = 0;

	while (i < arr.size() && diff < 3) {
		diff += (sorted[i] != arr[i]);
		i++;
	}

	return diff < 3;
}

int main(void) {
	cout << "Enter number of elements in the array followed by each element" << endl;
	cout << "> ";

	vector<int>::size_type elems;
	while (cin >> elems) {
		vector<int> arr;
		for (vector<int>::size_type i = 0; i < elems; i++) {
			int v;
			cin >> v;
			arr.push_back(v);
		}

		if (sort_1swap(arr)) {
			cout << "Can be sorted with one swap only." << endl;
		} else {
			cout << "Cannot be sorted with one swap." << endl;
		}

		cout << "> ";
	}

	return 0;
}
