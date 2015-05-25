
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
