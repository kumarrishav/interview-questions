/* A magnitude pole of an array A consisting of N integers is an index K such that all elements with
 * smaller indexes have values lower than or equal to A[K] and all elements with greater indexes
 * have values greater than or equal to A[K].
 *
 * For example, for A = [ 4, 2, 2, 3, 1, 4, 7, 8, 6, 9 ], there is a magnitude pole for K = 5,
 * since A[0..4] <= A[5] <= A[6..9]. Furthermore, the array doesn't have any other magnitude poles.
 *
 * Write a function, magnitude_pole(), that given an array, returns the index of any of its
 * magnitude poles.
 *
 * Source: Careercup
 */
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

vector<int>::const_iterator magnitude_pole(const vector<int> &arr) {

	if (arr.size() == 0) {
		return arr.end();
	}

	vector<int> mins(arr.size());
	vector<int> maxs(arr.size());

	mins[arr.size()-1] = numeric_limits<int>::max();
	maxs[0] = numeric_limits<int>::min();

	for (vector<int>::size_type i = 1; i < arr.size(); i++) {
		mins[arr.size()-1-i] = min(mins[arr.size()-i], arr[arr.size()-i]);
		maxs[i] = max(maxs[i-1], arr[i-1]);
	}

	for (vector<int>::size_type i = 1; i < arr.size()-1; i++) {
		if (maxs[i] <= arr[i] && mins[i] >= arr[i]) {
			return arr.begin()+i;
		}
	}

	return arr.end();
}

int main(void) {
	cout << "Enter number of elements in array followed by each element" << endl;
	cout << "> ";

	vector<int>::size_type elems;
	while (cin >> elems) {
		vector<int> array(elems);
		for (vector<int>::size_type i = 0; i < elems; i++) {
			cin >> array[i];
		}
		vector<int>::const_iterator mag_pole = magnitude_pole(array);
		if (mag_pole == array.end()) {
			cout << "No magnitude pole found." << endl;
		} else {
			cout << "Magnitude pole found at index " << mag_pole-array.begin() << endl;
		}
		cout << "> ";
	}

	return 0;
}
