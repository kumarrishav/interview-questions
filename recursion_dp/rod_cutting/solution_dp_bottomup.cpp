/* You are given a piece of wood of length N, and an array of N elements, where A[i] stores the
 * price of a piece of wood of length i+1 (0 <= i < N).
 *
 * Write a function that finds the optimal cutting sequence in order to maximize profit.
 *
 */

#include <iostream>
#include <algorithm>
#include <vector>

/* Bottom up DP. A lot faster than the naive approach. Also does away with the recursion,
 * eliminating the possibility of stack overflow. Nice!
 */

using namespace std;

typedef vector<int>::size_type size_type;

int cut_rod(const vector<int> &prices) {
	vector<int> dp(prices.size()+1);

	dp[0] = 0;

	for (size_type l = 1; l <= prices.size(); l++) {
		dp[l] = dp[0]+prices[l-1];
		for (size_type i = 1; i < l; i++) {
			dp[l] = max(dp[l], dp[i]+prices[l-i-1]);
		}
	}

	return dp[prices.size()];
}

int main(void) {
	cout << "Enter size of rod followed by the prices array." << endl;
	cout << "> ";

	size_type rod_len;

	while (cin >> rod_len) {
		vector<int> prices(rod_len);

		for (size_type i = 0; i < rod_len; i++) {
			cin >> prices[i];
		}

		cout << "Best value = " << cut_rod(prices) << endl;
		cout << "> ";
	}

	return 0;
}
