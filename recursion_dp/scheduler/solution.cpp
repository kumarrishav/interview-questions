/* Given a single core processor and 2 processes executing programs with M and N atomic
 * instructions, write a program to count how many different schedulings are possible.
 *
 * EXAMPLE
 *
 * Input:
 * M = 2, N = 1 (Instructions M1, M2 for process 1 and N1 for process 2)
 *
 * Output:
 * 3
 *
 * Explanation:
 * The possibilities are:
 * { M1, M2, N1 }
 * { M1, N1, M2 }
 * { N1, M2, M2 }
 *
 * Source: Careercup
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

unsigned scheduler_possibilities(unsigned m, unsigned n) {
	if (m == 0 || n == 0) {
		return 1;
	}
	return scheduler_possibilities(m-1, n)+scheduler_possibilities(m, n-1);
}

unsigned scheduler_possibilities_dp(unsigned m, unsigned n) {
	unsigned dim = max(m+1, n+1);
	vector<vector<unsigned> > dp(dim, vector<unsigned>(dim));

	for (size_t i = 0; i < dim; i++) {
		dp[i][0] = 1;
		dp[0][i] = 1;
	}

	for (size_t i = 1; i < dim; i++) {
		for (size_t j = 1; j < dim; j++) {
			dp[i][j] = dp[i-1][j]+dp[i][j-1];
		}
	}

	return dp[m][n];
}

int main(void) {
	cout << "Enter M and N" << endl;
	cout << "> ";

	unsigned m, n;
	while (cin >> m >> n) {
		// naive recursion will take forever with big values
		// cout << scheduler_possibilities(m, n) << endl;
		cout << scheduler_possibilities_dp(m, n) << endl << "> ";
	}

	return 0;
}
