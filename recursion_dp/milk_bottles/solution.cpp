/* A milkman serves milk in packaged bottles of varied sizes. The different bottle sizes are
 * 1, 5, 7 and 10.
 *
 * He wants to supply the desired quantity of milk using as less bottles as possible.
 *
 * Write a function to help the milkman finding the minimum number of bottles required to supply the
 * necessary milk. Your function receives the milk quantity, L, and returns the minimum number of
 * bottles.
 *
 * EXAMPLE
 *
 * Input:  17 (L = 17)
 *
 * Output: 2
 *
 * Explanation: The optimal choice is to use a bottle of size 7 and a bottle of size 10.
 *
 * Source: StackOverflow
 */

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

unsigned min_bottles(unsigned L) {
	vector<unsigned> dp(L+1);
	dp[0] = 0;

	for (vector<unsigned>::size_type i = 1; i < L+1; i++) {
		dp[i] = dp[i-1]+1;
		if (i > 4)
			dp[i] = min(dp[i], dp[i-5]+1);
		if (i > 6)
			dp[i] = min(dp[i], dp[i-7]+1);
		if (i > 9)
			dp[i] = min(dp[i], dp[i-10]+1);
	}

	return dp[L];
}

unsigned bottles_brute_force(unsigned L) {
	if (L == 0)
		return 0;
	unsigned res = bottles_brute_force(L-1)+1;
	if (L > 4)
		res = min(res, bottles_brute_force(L-5)+1);
	if (L > 6)
		res = min(res, bottles_brute_force(L-7)+1);
	if (L > 9)
		res = min(res, bottles_brute_force(L-10)+1);
	return res;
}

int main(void) {
	cout << "Enter the demand of milk." << endl;
	cout << "> ";

	unsigned L;
	while (cin >> L) {

		/* Brute force solution does not terminate timely for big input values.
		 * It was just used to test the correctness of the DP solution.
		 */

		// cout << "Brute force: " << bottles_brute_force(L) << endl;

		cout << "DP: " << min_bottles(L) << endl;
		cout << "> ";
	}

	return 0;
}
