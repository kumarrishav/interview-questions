
#include <iostream>
#include <vector>

using namespace std;

unsigned long long paths(unsigned N, unsigned M) {
	vector<vector<unsigned long long> > dp(M, vector<unsigned long long>(N));

	for (vector<vector<unsigned long long> >::size_type i = 0; i < M; i++) {
		dp[i][0] = 1;
	}
	for (vector<unsigned long long>::size_type i = 0; i < N; i++) {
		dp[0][i] = 1;
	}

	for (vector<vector<unsigned long long> >::size_type row = 1; row < M; row++) {
		for (vector<unsigned long long>::size_type col = 1; col < N; col++) {
			dp[row][col] = dp[row][col-1]+dp[row-1][col];
		}
	}

	return dp[M-1][N-1];
}

int main(void) {
	cout << "Enter N and M for a rectangle with width = N and height = M" << endl;
	cout << "> ";

	unsigned N, M;
	while (cin >> N >> M) {
		cout << "Paths = " << paths(N, M) << endl;
		cout << "> ";
	}

	return 0;
}
