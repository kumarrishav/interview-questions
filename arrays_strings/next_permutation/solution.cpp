/* Implement a function, next_perm(), that receives an array and returns the next permutation
 * of that array. The next permutation of an array is the smallest array with the same elements
 * that is considered to be greater than the original array.
 *
 * Do it inplace with O(1) memory and O(n) time.
 *
 * EXAMPLES
 *
 * Input: [ 1, 2, 3, 4 ]
 * Output: [1, 2, 4, 3 ]
 *
 * Input: [ 1, 3, 4, 2 ]
 * Output: [ 1, 4, 2, 3 ]
 *
 * Source: StackOverflow
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

bool next_perm(vector<int> &array) {
	if (array.size() <= 1) {
		return false;
	}

	vector<int>::size_type i;
	for (i = array.size()-1; i > 0 && array[i] <= array[i-1]; i--);

	if (i == 0) {
		return false;
	}

	i--;

	vector<int>::size_type j;
	for (j = array.size()-1; array[j] <= array[i]; j--);

	swap(array[i], array[j]);
	reverse(array.begin()+i+1, array.end());

	return true;
}

void print_arr(const vector<int> &array) {

	cout << "[ ";

	if (array.size() > 0) {
		cout << array[0];
	}

	for (vector<int>::size_type i = 1; i < array.size(); i++) {
		cout << ", " << array[i];
	}

	cout << "] ";
}

int main(void) {
	cout << "Enter size of array, followed by the elements to see the sequence of increasing permutations" << endl;
	cout << "> ";

	vector<int>::size_type arr_sz;
	while (cin >> arr_sz) {
		vector<int> array(arr_sz);
		for (vector<int>::size_type i = 0; i < arr_sz; i++) {
			cin >> array[i];
		}

		while (next_perm(array)) {
			print_arr(array);
			cout << endl;
		}

		cout << "> ";
	}

	cout << endl;
}
