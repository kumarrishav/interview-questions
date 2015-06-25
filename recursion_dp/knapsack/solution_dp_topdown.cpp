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

typedef vector<unsigned>::size_type size_type;

static int knapsack_aux(const vector<unsigned> &values, const vector<unsigned> &weights,
			unsigned capacity, size_type items, vector<vector<int> > &dp) {

	if (items == 0) {
		return 0;
	}

	if (dp[capacity][items] != -1) {
		return dp[capacity][items];
	}

	int res = knapsack_aux(values, weights, capacity, items-1, dp);
	if (weights[items-1] <= capacity) {
		res = max((unsigned) res, values[items-1]+knapsack_aux(values, weights, capacity-weights[items-1], items-1, dp));
	}

	dp[capacity][items] = res;

	return res;
}

int knapsack(const vector<unsigned> &values, const vector<unsigned> &weights, unsigned capacity) {
	vector<vector<int> > dp(capacity+1, vector<int>(values.size()+1, -1));
	return knapsack_aux(values, weights, capacity, values.size(), dp);
}

int main(void) {
	cout << "Enter number of items, followed by the weights, followed by the values, followed by capacity" << endl;
	cout << "> ";

	size_type items;
	while (cin >> items) {
		vector<unsigned> values(items), weights(items);
		for (size_type i = 0; i < items; i++) {
			cin >> weights[i];
		}
		for (size_type i = 0; i < items; i++) {
			cin >> values[i];
		}
		unsigned cap;
		cin >> cap;
		cout << knapsack(values, weights, cap) << endl;
		cout << "> ";
	}

	return 0;
}
