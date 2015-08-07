
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

static unsigned min_penalty(const vector<unsigned> &jump_p, const vector<unsigned> steps[2],
			    unsigned stair, vector<unsigned>::size_type stair_i) {
	if (stair_i == steps[0].size())
		return 0;

	unsigned other_stair = (stair+1)%2;

	unsigned jump_res = jump_p[stair_i]+steps[other_stair][stair_i];
	jump_res += min_penalty(jump_p, steps, other_stair, stair_i+1);

	unsigned nojump_res = steps[stair][stair_i]+min_penalty(jump_p, steps, stair, stair_i+1);

	return min(jump_res, nojump_res);
}

unsigned min_penalty_brute_force(const vector<unsigned> &jump_p, const vector<unsigned> steps[2]) {
	return min(min_penalty(jump_p, steps, 0, 0), min_penalty(jump_p, steps, 1, 0));
}

unsigned min_penalty_dp(const vector<unsigned> &jump_p, const vector<unsigned> steps[2]) {

	vector<vector<unsigned> > dp_table(2, vector<unsigned>(steps[0].size()+1));
	int n = steps[0].size()+1;

	dp_table[0][n-1] = 0;
	dp_table[1][n-1] = 0;

	for (int i = n-2; i >= 0; i--)
		for (int j = 0; j < 2; j++)
			dp_table[j][i] = min(dp_table[j][i+1]+steps[j][i],
					     dp_table[(j+1)%2][i+1]+jump_p[i]+steps[(j+1)%2][i]);

	return min(dp_table[0][0], dp_table[1][0]);
}

int main(void) {

	cout << "Enter the number of stairs N." << endl;
	cout << "Then, enter an array J of size N where J[i] is the cost of jumping from step i in one stair to step i in the other stair." << endl;
	cout << "Then, enter an array L of size N-1 where L[i] is the cost of going from step i to step i+1 on the left stair." << endl;
	cout << "Then, enter an array R of size N-1 where R[i] is the cost of going from step i to step i+1 on the right stair." << endl;

	vector<unsigned> steps[2];
	vector<unsigned> jumps;
	vector<unsigned>::size_type N;

	while (cin >> N) {

		steps[0].clear();
		steps[1].clear();
		jumps.clear();

		for (vector<unsigned>::size_type i = 0; i < N; i++) {
			unsigned v;
			cin >> v;
			jumps.push_back(v);
		}

		for (vector<unsigned>::size_type i = 0; i < N-1; i++) {
			unsigned v;
			cin >> v;
			steps[0].push_back(v);
		}

		for (vector<unsigned>::size_type i = 0; i < N-1; i++) {
			unsigned v;
			cin >> v;
			steps[1].push_back(v);
		}

		// cout << "Brute force: " << min_penalty_brute_force(jumps, steps) << endl;
		cout << "DP: " << min_penalty_dp(jumps, steps) << endl;

		cout << "> ";
	}

	return 0;
}
