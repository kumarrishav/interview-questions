/* You are given an array of elements which can have duplicates.
 *
 * Write a function that finds and prints every possible sub-array.
 *
 * Each sub-array can contain any number of elements of the original array, in any order.
 * Do not print the same sub-array more than once. A sub-array is considered duplicate if it contains
 * exactly the same elements with the same frequency counts (order is irrelevant). So,
 * [ 4, 2, 3, 1 ] and [ 1, 4, 2, 3 ] are seen as duplicates.
 *
 * EXAMPLE
 *
 * Input: [ 1, 2, 3, 1 ]
 * Output:
 * [ ]
 * [ 1 ]
 * [ 2 ]
 * [ 3 ]
 * [ 1, 1 ]
 * [ 1, 2 ]
 * [ 1, 3 ]
 * [ 2, 3 ]
 * [ 1, 1, 2 ]
 * [ 1, 1, 3 ]
 * [ 1, 2, 3 ]
 * [ 1, 1, 2, 3 ]
 *
 * You can print the arrays in any order
 *
 * Source: Careercup
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

static void powerset_aux(const vector<int> &elems, vector<int>::size_type elems_i, vector<int> &out_buff) {
	if (elems_i >= elems.size()) {
		cout << "{ ";
		if (out_buff.size() > 0) {
			cout << out_buff[0];
		}
		for (vector<int>::size_type i = 1; i < out_buff.size(); i++) {
			cout << ", " << out_buff[i];
		}
		cout << " }" << endl;
		return;
	}

	vector<int>::size_type next_different;
	for (next_different = elems_i+1;
	     next_different < elems.size() && elems[next_different] == elems[elems_i];
	     next_different++)
		; /* Intentionally left blank */

	powerset_aux(elems, next_different, out_buff);

	for (vector<int>::size_type next_eq = elems_i; next_eq != next_different; next_eq++) {
		out_buff.push_back(elems[next_eq]);
		powerset_aux(elems, next_different, out_buff);
	}

	for (vector<int>::size_type i = elems_i; i != next_different; i++) {
		out_buff.pop_back();
	}
}

void powerset(vector<int> &elems) {

	sort(elems.begin(), elems.end());

	vector<int> buff;
	powerset_aux(elems, 0, buff);
}

int main(void) {
	cout << "Enter the number of elements, followed by each element, to generate the power set." << endl;
	cout << "> ";

	vector<int>::size_type elems_no;
	while (cin >> elems_no) {
		vector<int> elems;
		for (vector<int>::size_type i = 0; i < elems_no; i++) {
			int element;
			cin >> element;
			elems.push_back(element);
		}
		powerset(elems);
		cout << "> ";
	}

	return 0;
}
