/* You have a knapsack that can take at most a total weight of W.
 * There is a set of N items, and each item i weighs w[i] and is worth v[i].
 *
 * Write a function that computes the largest possible value that can be carried
 * in the knapsack without exceeding its capacity.
 *
 */
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int knapsack(const vector<unsigned> &values, const vector<unsigned> &weights, unsigned capacity) {
	vector<vector<unsigned> > dp(values.size()+1, vector<unsigned>(capacity+1));

	for (unsigned j = 0; j <= capacity; j++) {
		dp[0][j] = 0;
	}

	for (unsigned i = 1; i <= values.size(); i++) {
		for (unsigned j = 0; j <= capacity; j++) {
			dp[i][j] = dp[i-1][j];
			if (weights[i-1] <= j) {
				dp[i][j] = max(dp[i][j], dp[i-1][j-weights[i-1]]+values[i-1]);
			}
		}
	}

	return dp[values.size()][capacity];
}

int main(void) {
	cout << "Enter number of items, followed by the weights, followed by the values, followed by capacity" << endl;
	cout << "> ";

	vector<unsigned>::size_type items;
	while (cin >> items) {
		vector<unsigned> values(items), weights(items);
		for (vector<unsigned>::size_type i = 0; i < items; i++) {
			cin >> weights[i];
		}
		for (vector<unsigned>::size_type i = 0; i < items; i++) {
			cin >> values[i];
		}
		unsigned cap;
		cin >> cap;
		cout << knapsack(values, weights, cap) << endl;
		cout << "> ";
	}

	return 0;
}
