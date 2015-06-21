/* Given a set of non-overlapping intervals, write a function to return the new set of intervals
 * after insert a new interval, coalescing when possible.
 *
 * Assume that the intervals were initially sorted according to their start times.
 *
 * EXAMPLE 1:
 *
 * Input sequence: [1, 3], [6,9]
 * Interval to insert: [2, 5]
 * Result: [1, 5], [6, 9]
 *
 * EXAMPLE 2:
 *
 * Input sequence: [1, 2], [3, 5], [6, 7], [8, 10], [12, 16]
 * Interval to insert: [4, 9]
 * Result: [1, 2], [3, 10], [12, 16]
 *
 * Source: Careercup (LinkedIn interview)
 */

#include <iostream>
#include <vector>

using namespace std;

struct Interval {
	int start;
	int end;
	Interval() : start(0), end(0) {}
	Interval(int s, int e) : start(s), end(e) {}
};

/* Finds the last interval I from a sorted array of intervals s.t. I.start <= time
 * O(log(N))
 */
int find_interval(const vector<Interval> &intervals, int time) {
	int l = -1;
	int r = intervals.size();
	while (l+1 != r) {
		int m = l+(r-l)/2;
		if (intervals[m].start <= time) {
			l = m;
		} else {
			r = m;
		}
	}

	if (l == -1 || intervals[l].start > time) {
		return -1;
	} else {
		return l;
	}
}

vector<Interval> insert(const vector<Interval> &intervals, const Interval &new_interval) {
	vector<Interval> res;
	int before, after;
	before = find_interval(intervals, new_interval.start);
	after = find_interval(intervals, new_interval.end);

	for (int i = 0; i < before; i++) {
		res.push_back(intervals[i]);
	}

	int nstart, nend;

	if (before != -1 && new_interval.start > intervals[before].end) {
		res.push_back(intervals[before]);
		nstart = new_interval.start;
	} else {
		nstart = (before != -1 ? intervals[before].start : new_interval.start);
	}

	if (after != -1 && new_interval.end < intervals[after].end) {
		nend = intervals[after].end;
	} else {
		nend = new_interval.end;
	}

	res.push_back(Interval(nstart, nend));

	for (int i = after+1; i < (int) intervals.size(); i++) {
		res.push_back(intervals[i]);
	}

	return res;
}

void print_intervals(const vector<Interval> &intervals) {
	if (intervals.size() > 0) {
		cout << "[" << intervals[0].start << ", " << intervals[0].end << "]";
	}
	for (vector<int>::size_type i = 1; i < intervals.size(); i++) {
		cout << ", [" << intervals[i].start << ", " << intervals[i].end << "]";
	}
}

int main(void) {
	cout << "Enter a sorted array of non-overlapping intervals as follows:" << endl;
	cout << "First, indicate the number of intervals (line 1)" << endl;
	cout << "Then, for each interval, indicate the start and end values" << endl;
	cout << "Then, indicate the interval to insert" << endl;
	cout << "> ";

	vector<int>::size_type intervals_sz;
	while (cin >> intervals_sz) {
		vector<Interval> intervals(intervals_sz);
		for (vector<Interval>::size_type i = 0; i < intervals_sz; i++) {
			cin >> intervals[i].start;
			cin >> intervals[i].end;
		}

		Interval to_ins;
		cin >> to_ins.start;
		cin >> to_ins.end;

		cout << "Inserting [" << to_ins.start << ", " << to_ins.end << "] into:" << endl;
		print_intervals(intervals);
		cout << endl;

		vector<Interval> new_set = insert(intervals, to_ins);

		cout << "After insertion:" << endl;
		print_intervals(new_set);
		cout << endl << "> ";
	}

	return 0;
}
