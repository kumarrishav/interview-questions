
#include <iostream>
#include <vector>

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
