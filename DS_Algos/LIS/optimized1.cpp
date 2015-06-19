/* An O(N log(N)) algorithm to find the Longest Increasing Subsequence using a
 * modified Patience Sorting.
 *
 * The algorithm works as follows: first, run a modified patience sorting to create the piles.
 * The modified version keeps track of paths along the piles as new nodes are inserted: when a node
 * is pushed into pile `i`, that node will store a pointer to the top node of pile `i-1`.
 *
 * When patience sort ends, the size of the Longest Increasing Subsequence is equal to the number of
 * piles. To reconstruct the subsequence, start from the top node in the last pile, and follow the
 * pointers backwards until the first pile. The elements traversed constitude the Longest Increasing
 * Subsequence. When traversing this path, push each element into the output result array, and then
 * reverse the array.
 *
 * Note that a naive linear scan along the piles when performing patience sorting blows the
 * O(N log(N)) bound. Instead of doing this, since the top elements of each pile are
 * in increasing order from left to right, we perform binary search at each step to find
 * the leftmost pile such that its top is greater than the node we want to insert, thus bringing
 * the complexity to the desired O(N log(N)) bound.
 */
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct LISElement {
	int value;
	vector<int>::size_type prev_pile_idx;
};

class LongestIncreasingSubsequence {
private:
	vector<vector<LISElement> > piles;

	vector<vector<LISElement> >::iterator find_pile(int val) {
		int l = -1;
		int r = piles.size();

		while (l+1 != r) {
			int m = l+(r-l)/2;
			if (piles[m][piles[m].size()-1].value <= val) {
				l = m;
			} else {
				r = m;
			}
		}

		return piles.begin()+r;
	}

	void patience_sort(const vector<int> &array) {
		for (vector<int>::size_type i = 0; i < array.size(); i++) {

			vector<vector<LISElement> >::iterator pile_it = find_pile(array[i]);
			vector<vector<LISElement> >::size_type pile_idx;

			LISElement elem;
			elem.value = array[i];

			if (pile_it == piles.end()) {
				piles.push_back(vector<LISElement>());
				pile_idx = piles.size()-1;
			} else {
				pile_idx = pile_it-piles.begin();
			}

			if (pile_idx > 0) {
				elem.prev_pile_idx = piles[pile_idx-1].size()-1;
			}

			piles[pile_idx].push_back(elem);
		}
	}

public:

	void solve(const vector<int> &array, vector<int> &out) {
		piles.clear();
		patience_sort(array);

		LISElement &curr = piles[piles.size()-1][0];
		out.push_back(curr.value);
		for (vector<vector<int> >::size_type i = 1; i < piles.size(); i++) {
			curr = piles[piles.size()-1-i][curr.prev_pile_idx];
			out.push_back(curr.value);
		}

		reverse(out.begin(), out.end());
	}

};

int main(void) {

	cout << "Enter size of the array, followed by the elements" << endl;
	cout << "> ";

	LongestIncreasingSubsequence lis_solver;

	vector<int>::size_type array_sz;
	while (cin >> array_sz) {
		vector<int> array;
		for (vector<int>::size_type i = 0; i < array_sz; i++) {
			int val;
			cin >> val;
			array.push_back(val);
		}

		vector<int> lis;
		lis_solver.solve(array, lis);

		cout << "LIS = [ " << lis[0];
		for (vector<int>::size_type i = 1; i < lis.size(); i++) {
			cout << ", " << lis[i];
		}
		cout << " ]" << endl;

		cout << "> ";		
	}

	return 0;
}
