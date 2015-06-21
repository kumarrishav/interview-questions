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
 * The solution in this file assumes that one can go in any direction - left, right, up, down. So,
 * the rectangle is conceptually an undirected cyclic graph where node (i, j) is connected to
 * nodes (i, j-1), (i, j+1), (i-1, j) and (i+1, j).
 *
 * The number of possible paths explodes and grows very quickly; the code is O(4^(NM)).
 */

using namespace std;

unsigned long long paths_aux(int src_x, int src_y, int dst_x, int dst_y,
			     int N, int M, vector<vector<bool> > &visited) {

	if (src_x < 0 || src_x >= M || src_y < 0 || src_y >= N || visited[src_x][src_y]) {
		return 0;
	}

	if (src_x == dst_x && src_y == dst_y) {
		return 1;
	}

	visited[src_x][src_y] = true;

	unsigned long long res = 0;
	res += paths_aux(src_x, src_y-1, dst_x, dst_y, N, M, visited);
	res += paths_aux(src_x, src_y+1, dst_x, dst_y, N, M, visited);
	res += paths_aux(src_x-1, src_y, dst_x, dst_y, N, M, visited);
	res += paths_aux(src_x+1, src_y, dst_x, dst_y, N, M, visited);

	visited[src_x][src_y] = false;

	return res;
}

unsigned long long paths(int N, int M) {
	vector<vector<bool> > visited(M, vector<bool>(N));
	return paths_aux(0, 0, M-1, N-1, N, M, visited);
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
