/* You are given a piece of wood of length N, and an array of N elements, where A[i] stores the
 * price of a piece of wood of length i+1 (0 <= i < N).
 *
 * Write a function that finds the optimal cutting sequence in order to maximize profit.
 *
 */

#include <iostream>
#include <algorithm>
#include <vector>

/* Top down DP optimization. Much faster than naive approach.
 * We can convert it into bottom up easily. Bottom up has the advantage
 * of not suffering with stack overflow.
 */

using namespace std;

typedef vector<int>::size_type size_type;

static int cut_rod_aux(const vector<int> &prices, int len, vector<int> &dp) {
	if (len <= 0) {
		return 0;
	}

	if (dp[len] != -1) {
		return dp[len];
	}

	int res = prices[0]+cut_rod_aux(prices, len-1, dp);

	for (int i = 1; i < len; i++) {
		res = max(res, prices[i]+cut_rod_aux(prices, len-i-1, dp));
	}

	dp[len] = res;

	return res;
}

int cut_rod(const vector<int> &prices) {
	vector<int> dp(prices.size()+1, -1);
	return cut_rod_aux(prices, prices.size(), dp);
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
