/* You have an M by N rectangle formed by nodes. You can think of the rectangle as a set of layers
 * of nodes, where the outermost layer defines the rectangle, and then each inner layer is also
 * filled with nodes.
 *
 * For example, for M = 5 and N = 3, we would have the following nodes:
 *
 * O O O O O
 * O O O O O
 * O O O O O
 *
 * Let A be the upper left node. Let B be the bottom right node.
 * Design and implement an algorithm that finds the number of paths from A to B.
 *
 * Source: Careercup
 */
#include <iostream>
#include <vector>

/* This question is deliberately underspecified to see if the candidate asks good
 * questions before rushing to code.
 *
 * One important question to ask is how are nodes connected, that is, where can we go
 * from a node?
 *
 * The solution in this file assumes that one can only go right and down. So, the rectangle
 * is conceptually an acyclic directed graph. For example, with M = 5 and N = 3, nodes are
 * connected like so:
 *
 *  O -> O -> O -> O -> O
 *  |    |    |    |    |
 * `'´  `'´  `'´  `'´  `'´
 *  O -> O -> O -> O -> O
 *  |    |    |    |    |
 * `'´  `'´  `'´  `'´  `'´
 *  O -> O -> O -> O -> O
 *
 * Under these assumptions, the problem can be solved in O(MN) using a dynamic programming table
 * where table[i][j] stores the number of paths from A to node (i, j) in the rectangle.
 *
 * See solution2.cpp for the case where any node connects to its four neighbors.
 *
 */

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
