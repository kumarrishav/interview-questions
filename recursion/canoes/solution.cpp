/* A canoe can hold 2 kids and a maximum of 150 lbs.
 * Given a list of each kid's weight, write a function that computes the minimum number of canoes
 * needed to take all the kids on a trip.
 *
 * Source: Careercup (Google interview)
 */
#include <iostream>
#include <algorithm>
#include <climits>
#include <vector>
#define MAX_CANOE_WEIGHT 150

using namespace std;

unsigned min_canoes(vector<unsigned> &kids) {
	if (kids.size() == 0) {
		return UINT_MAX;
	}

	sort(kids.begin(), kids.end());

	if (kids[kids.size()-1] > MAX_CANOE_WEIGHT) {
		return UINT_MAX;
	}

	vector<unsigned>::size_type low = 0;
	vector<unsigned>::size_type high = kids.size()-1;
	unsigned res = 0;

	while (low < high) {
		if (kids[low]+kids[high] <= MAX_CANOE_WEIGHT) {
			res++;
			low++;
			high--;
		} else {
			res++;
			high--;
		}
	}

	return res+(low == high);
}

int main(void) {
	cout << "Enter the number of kids, followed by each kid's weight" << endl;
	cout << "> ";

	vector<unsigned>::size_type kids_sz;
	while (cin >> kids_sz) {
		vector<unsigned> kids;
		for (vector<unsigned>::size_type i = 0; i < kids_sz; i++) {
			unsigned weight;
			cin >> weight;
			kids.push_back(weight);
		}
		unsigned res = min_canoes(kids);
		if (res == UINT_MAX) {
			cout << "Impossible" << endl;
		} else {
			cout << "Min canoes: " << res << endl;
		}
		cout << "> ";
	}

	return 0;
}
