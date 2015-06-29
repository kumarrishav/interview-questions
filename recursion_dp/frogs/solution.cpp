/* A lake contains a rectangular grid of stones evenly separated.
 *
 * Each stone has an attribute K that represents the maximum jump distance that a frog
 * can achieve when attempting to jump from that stone to another stone.
 *
 * Formally, given a stone (i, j) with some value K, the farthest stones that the frog can reach
 * with a single jump are those at (i+di, j+dj) such that abs(di)+abs(dj) = K.
 *
 * For example, if the stone at (2, 3) has K = 2, then the farthest stones reachable within one jump
 * are (2, 3+2) = (2, 5); (2, 3-2) = (2, 1); (2+1, 3+1) = (3, 4); (2+1, 3-1) = (3, 2);
 * (2-1, 3+1) = (1, 4); (2-1, 3-1) = (1, 2); (2+2, 3) = (4, 3) and (2-2, 3) = (0, 3)
 *
 * Write a program that finds and prints the path that minimizes the number of jumps to go from a
 * source stone to a destination stone.
 *
 * The frog is not allowed to go outside the bounds of the stones grid. Furthermore, the frog
 * doesn't necessarily have to jump the maximum distance K on each stone.
 *
 * Source: Careercup
 */

#include <iostream>
#include <vector>

using namespace std;

struct stone {
	unsigned x;
	unsigned y;
	int k;
	bool visited;
};

static void min_jumps_aux(vector<vector<stone> > &map,
			  int curr_x, int curr_y, int dest_x, int dest_y,
			  vector<stone> &res_buff, vector<stone> &result) {

	if (curr_x < 0 || curr_x >= (int) map.size() || curr_y < 0 || curr_y >= (int) map[0].size() ||
	    map[curr_x][curr_y].visited) {
		return;
	}

	if (curr_x == dest_x && curr_y == dest_y) {
		if (result.size() == 0 || res_buff.size()+1 < result.size()) {
			result = res_buff;
			result.push_back(map[curr_x][curr_y]);
		}
		return;
	}

	res_buff.push_back(map[curr_x][curr_y]);
	map[curr_x][curr_y].visited = true;

	for (int jump = 1; jump <= map[curr_x][curr_y].k; jump++) {
		for (int d = 0; d <= jump; d++) {
			int dx = d;
			int dy = jump-dx;
			min_jumps_aux(map, curr_x+dx, curr_y+dy, dest_x, dest_y, res_buff, result);
			min_jumps_aux(map, curr_x+dx, curr_y-dy, dest_x, dest_y, res_buff, result);
			min_jumps_aux(map, curr_x-dx, curr_y+dy, dest_x, dest_y, res_buff, result);
			min_jumps_aux(map, curr_x-dx, curr_y-dy, dest_x, dest_y, res_buff, result);
		}
	}

	map[curr_x][curr_y].visited = false;
	res_buff.pop_back();
}

void min_jumps(vector<vector<stone> > &map, vector<stone> &result,
	       int from_x, int from_y, int target_x, int target_y) {

	result.clear();

	for (vector<vector<stone> >::size_type i = 0; i < map.size(); i++) {
		for (vector<stone>::size_type j = 0; j < map[0].size(); j++) {
			map[i][j].visited = false;
		}
	}

	vector<stone> buff;

	min_jumps_aux(map, from_x, from_y, target_x, target_y, buff, result);
}

int main(void) {
	cout << "Enter lake dimensions followed by a K for each grid element." << endl;
	cout << "Then, enter the starting and ending positions for the frog." << endl;
	cout << "> ";

	vector<vector<stone> >::size_type lake_x, lake_y;
	while (cin >> lake_x >> lake_y) {
		vector<vector<stone> > lake(lake_x, vector<stone>(lake_y));
		for (unsigned i = 0; i < lake_x; i++) {
			for (unsigned j = 0; j < lake_y; j++) {
				cin >> lake[i][j].k;
				lake[i][j].x = i;
				lake[i][j].y = j;
			}
		}

		int start_x, start_y;
		int end_x, end_y;

		cin >> start_x >> start_y;
		cin >> end_x >> end_y;

		vector<stone> bestpath;
		min_jumps(lake, bestpath, start_x, start_y, end_x, end_y);

		if (bestpath.size() > 0) {
			cout << "(" << bestpath[0].x << ", " << bestpath[0].y << ")";
		}

		for (vector<stone>::size_type i = 1; i < bestpath.size(); i++) {
			cout << " -> (" << bestpath[i].x << ", " << bestpath[i].y << ")";
		}

		cout << endl << "> ";
	}

	return 0;
}
