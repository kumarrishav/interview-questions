/* You have a very large array of integers.
 *
 * How would you go about finding if there are 2 elements in the array that sum to a
 * given value?
 *
 * Source: Careercup (Facebook interview)
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

bool find_sum(vector<int> &arr, int sum) {
	sort(arr.begin(), arr.end());
	for (vector<int>::size_type i = 0; i < arr.size(); i++) {
		if (binary_search(arr.begin()+i+1, arr.end(), sum-arr[i])) {
			return true;
		}
	}
	return false;
}

int main(void) {
	cout << "Enter size of array, followed by the elements, followed by the target sum" << endl;
	cout << "> ";

	vector<int>::size_type elems;
	while (cin >> elems) {
		vector<int> arr;
		for (vector<int>::size_type i = 0; i < elems; i++) {
			int val;
			cin >> val;
			arr.push_back(val);
		}
		int sum;
		cin >> sum;

		if (find_sum(arr, sum)) {
			cout << "Yes." << endl;
		} else {
			cout << "No." << endl;
		}

		cout << "> ";
	}

	return 0;
}
