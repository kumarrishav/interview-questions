/* You have a pair of stairs, one is on your left, the other is on your right.
 * Both stairs have the same number of steps N.
 *
 * Your goal is to reach the end of either stairs (that is, reach step N of one of the stairs).
 *
 * There is a cost for going up one step on each stair. It is also possible to jump from one stair
 * to the other, but the jumps are horizontal, i.e., if you are on step i of the left stair and you
 * jump, then you will be on step i of the right stair. Each jump has a cost too.
 *
 * The costs of going up one step depend on the stair (left or right) and where in that stair you
 * are.
 *
 * The cost of horizontal jumps to switch stairs depends only on the step level.
 *
 * You are given 3 arrays with these costs:
 *
 * A[0..N-1], where A[i] is the cost of jumping to the other stair at step i.
 * B[0..N-2], where B[i] is the cost of going from step i to step i+1 on the left stair.
 * C[0..N-1], where C[i] is the cost of going from step i to step i+1 on the right stair.
 *
 * Write code that finds the minimum cost to reach the end of one of the stairs. You can start
 * on the first step of any stair. You can only go up one step at a time.
 *
 * Source: Careercup (Microsoft interview)
 */

#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

/* Brute force recursive solution. O(2^N) */
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

/* Dynamic programming (bottom-up) solution. Way more elegant. O(n) time and memory */
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

		/* Uncomment the next line to compare results. This was used to validate the
		 * correctness of the dynamic programming solution.
		 *
		 * The brute force solution does not terminate timely for big inputs because
		 * it has exponential running time.
		 */
 
		// cout << "Brute force: " << min_penalty_brute_force(jumps, steps) << endl;
		cout << "DP: " << min_penalty_dp(jumps, steps) << endl;

		cout << "> ";
	}

	return 0;
}
