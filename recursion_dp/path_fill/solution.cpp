/* You have a 5x5 grid numbered 1-25 and a set of 5 start-end point pairs.
 *
 * For each pair, you are to find a path that goes from the start point to the end point.
 *
 * The paths must meet some requirements:
 *
 * - Only horizontal and vertical moves are allowed
 * - Paths cannot cross. That is, if position (i, j) is part of a path, then no other path can
 *   have (i, j) as part of it.
 * - Taken together, all the disjoint paths must cover the entire grid.
 *
 * Assume that the input is such that there is always at least one valid solution. 
 *
 * EXAMPLE
 *
 * Consider the grid:
 *
 *  ---- ---- ---- ---- ----
 * | 1  | 2  | 3  | 4  | 5  |
 *  ---- ---- ---- ---- ----
 * | 6  | 7  | 8  | 9  | 10 |
 *  ---- ---- ---- ---- ----
 * | 11 | 12 | 13 | 14 | 15 |
 *  ---- ---- ---- ---- ----
 * | 16 | 17 | 18 | 19 | 20 |
 *  ---- ---- ---- ---- ----
 * | 21 | 22 | 23 | 24 | 25 |
 *  ---- ---- ---- ---- ----
 *
 * And the following pairs of points:
 *
 * (0, 0) -> (4, 1) (Go from 1 to 22)
 * (0, 3) -> (3, 1) (Go from 4 to 17)
 * (0, 4) -> (3, 2) (Go from 5 to 18)
 * (1, 3) -> (2, 2) (Go from 9 to 13)
 * (3, 4) -> (4, 2) (Go from 20 to 23)
 *
 * Then the only solution is:
 *
 * First pair:
 * (0, 0) -> (1, 0) -> (2, 0) -> (3, 0) -> (4, 0) -> (4, 1)
 *
 * Second pair:
 * (0, 3) -> (0, 2) -> (0, 1) -> (1, 1) -> (2, 1) -> (3, 1)
 *
 * Third pair:
 * (0, 4) -> (1, 4) -> (2, 4) -> (2, 3) -> (3, 3) -> (3, 2)
 *
 * Fourth pair:
 * (1, 3) -> (1, 2) -> (2, 2)
 *
 * Fifth pair:
 * (3, 4) -> (4, 4) -> (4, 3) -> (4, 2)
 *
 * This is the only solution because there is no other set of disjoint paths that covers the entire
 * grid and satisfies the start/end points
 *
 * Source: Careercup
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct point {
	int x;
	int y;
};

struct cell {
	int value;
	bool visited;
	cell() : value(0), visited(false) { }
};

static bool find_paths_aux(const vector<pair<point, point> > &points, vector<point>::size_type point_i,
			   vector<vector<cell> > &grid, vector<vector<point> > &paths_res);

static bool find_path_for(const vector<pair<point, point> > &points, vector<point>::size_type point_i,
			  vector<vector<cell> > &grid, int curr_x, int curr_y,
			  vector<vector<point> > &paths_res) {

	if (curr_x < 0 || curr_x >= (int) grid.size() || curr_y < 0 || curr_y >= (int) grid[0].size() ||
	    grid[curr_x][curr_y].visited) {
		return false;
	}

	grid[curr_x][curr_y].visited = true;

	if (curr_x == points[point_i].second.x && curr_y == points[point_i].second.y) {
		bool ret = false;

		if (find_paths_aux(points, point_i+1, grid, paths_res)) {
			paths_res.push_back(vector<point>(1, points[point_i].second));
			ret = true;
		}

		grid[curr_x][curr_y].visited = false;

		return ret;
	}

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if ((i == 0) != (j == 0)) {
				if (find_path_for(points, point_i, grid, curr_x+i, curr_y+j, paths_res)) {
					point p;
					p.x = curr_x;
					p.y = curr_y;
					paths_res[paths_res.size()-1].push_back(p);
					return true;
				}
			}
		}
	}

	grid[curr_x][curr_y].visited = false;

	return false;
}


static bool find_paths_aux(const vector<pair<point, point> > &points, vector<point>::size_type point_i,
			   vector<vector<cell> > &grid, vector<vector<point> > &paths_res) {

	if (point_i == points.size()) {
		unsigned used = 0;
		for (vector<vector<cell> >::const_iterator row_it = grid.begin();
		     row_it != grid.end();
		     row_it++) {
			for (vector<cell>::const_iterator col_it = row_it->begin();
			     col_it != row_it->end();
			     col_it++) {
				if (col_it->visited) {
					used++;
				}
			}
		}
		return used == grid.size()*grid[0].size();
	}

	return find_path_for(points, point_i, grid, points[point_i].first.x, points[point_i].first.y,
			     paths_res);
}

bool find_paths(const vector<pair<point, point> > &points, vector<vector<cell> > &grid, vector<vector<point> > &res) {

	for (vector<vector<cell> >::iterator row_it = grid.begin(); row_it != grid.end(); row_it++) {
		for (vector<cell>::iterator col_it = row_it->begin(); col_it != row_it->end(); col_it++) {
			col_it->visited = false;
		}
	}

	if (!find_paths_aux(points, 0, grid, res)) {
		return false;
	}

	reverse(res.begin(), res.end());

	for (vector<vector<point> >::iterator path_it = res.begin(); path_it != res.end(); path_it++) {
		reverse(path_it->begin(), path_it->end());
	}

	return true;
}

int main(void) {
	vector<vector<cell> > grid(5, vector<cell>(5));

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			grid[i][j].value = i*5+1+j;
		}
	}

	cout << "Enter the number of point pairs, followed by each pair" << endl;
	cout << "> ";

	vector<pair<point, point> >::size_type pts;
	while (cin >> pts) {
		vector<pair<point, point> > points;
		for (vector<pair<point, point> >::size_type i = 0; i < pts; i++) {
			point p1, p2;
			cin >> p1.x >> p1.y >> p2.x >> p2.y;
			points.push_back(pair<point, point>(p1, p2));
		}

		vector<vector<point> > res;
		if (!find_paths(points, grid, res)) {
			cout << "Impossible" << endl;
		} else {
			for (vector<vector<point> >::const_iterator path_it = res.begin();
			     path_it != res.end();
			     path_it++) {
				
				cout << "(" << (*path_it->begin()).x << ", " << (*path_it->begin()).y << ")";
				for (vector<point>::const_iterator points_it = path_it->begin()+1;
				     points_it != path_it->end();
				     points_it++) {
					cout << " -> (" << points_it->x << ", " << points_it->y << ")";
				}
				cout << endl;

			}
		}

		cout << "> ";
	}

	return 0;
}
