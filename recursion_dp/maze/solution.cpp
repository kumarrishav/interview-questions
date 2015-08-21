/* Consider a grid representing an NxM maze. Each position in the grid is either the character 'X'
 * or the character '_'. 'X' means that there is a wall in that position, '_' means that there is
 * nothing in there. We cannot go through walls when travelling inside the maze.
 *
 * You are given a starting position in the maze. To get out of the maze, it is enough to reach a
 * position in the borders (i.e., any position with x = 0, x = N, y = 0 or y = M, assuming x is
 * a zero-based index to select the row out of the N rows, and y is a zero-based index to select
 * the column out of the M columns in each row.
 *
 * Write a function that finds a path to get out of the maze. Print the path. If it's impossible
 * to get out of the maze, print "Impossible"
 *
 * Source: Careercup (EMC interview)
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

bool find_path_aux(const vector<vector<char> > &maze, vector<vector<bool> > &visited,
		   vector<string> &path, int curr_x, int curr_y) {

	if (curr_x < 0 || curr_x >= (int) maze.size() || curr_y < 0 || curr_y >= (int) maze[0].size())
		return true;

	if (visited[curr_x][curr_y] || maze[curr_x][curr_y] == 'X')
		return false;

	assert(maze[curr_x][curr_y] == '_');

	visited[curr_x][curr_y] = true;
	ostringstream oss;
	oss << "( " << curr_x << ", " << curr_y << " )";
	path.push_back(oss.str());

	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
			if ((i == 0) != (j == 0))
				if (find_path_aux(maze, visited, path, curr_x+i, curr_y+j))
					return true;

	path.pop_back();
	visited[curr_x][curr_y] = false;

	return false;
}

bool find_path(const vector<vector<char> > &maze, vector<string> &path, int pos_x, int pos_y) {
	path.clear();
	vector<vector<bool> > visited(maze.size(), vector<bool>(maze[0].size(), false));
	return find_path_aux(maze, visited, path, pos_x, pos_y);
}

int main(void) {
	cout << "Enter maze dimensions N and M for an NxM maze, followed by each entry separated by a blank," << endl;
	cout << "followed by the initial position" << endl;
	cout << "> ";

	unsigned rows, cols;
	while (cin >> rows >> cols) {
		vector<vector<char> > maze(rows, vector<char>(cols));
		for (unsigned i = 0; i < rows; i++)
			for (unsigned j = 0; j < cols; j++)
				cin >> maze[i][j];

		unsigned pos_x, pos_y;
		cin >> pos_x >> pos_y;

		vector<string> path;
		if (find_path(maze, path, pos_x, pos_y)) {
			if (path.size() > 0)
				cout << path[0];
			for (vector<string>::size_type i = 1; i < path.size(); i++)
				     cout << " -> " << path[i];
			     cout << endl;
		} else {
			cout << "Impossible" << endl;
		}

		cout << "> ";
	}

	return 0;
}
