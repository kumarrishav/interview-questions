
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

template <class T, class Cmp = less<T> >
class PriorityQueue {
public:
	typedef typename vector<T>::size_type size_type;
private:
	vector<T> min_heap;
	Cmp cmp;

	void sift_down(size_type curr) {
		size_type left = curr*2+1;
		size_type right = left+1;
		size_type smallest = curr;

		if (left < min_heap.size() && cmp(min_heap[left], min_heap[smallest])) {
			smallest = left;
		}
		if (right < min_heap.size() && cmp(min_heap[right], min_heap[smallest])) {
			smallest = right;
		}

		while (curr != smallest) {
			swap(min_heap[curr], min_heap[smallest]);

			curr = smallest;
			left = curr*2+1;
			right = left+1;

			if (left < min_heap.size() && cmp(min_heap[left], min_heap[smallest])) {
				smallest = left;
			}
			if (right < min_heap.size() && cmp(min_heap[right], min_heap[smallest])) {
				smallest = right;
			}
		}
	}

	void sift_up(size_type curr) {
		size_type parent = (curr+1)/2-1;
		while (curr != 0 && cmp(min_heap[curr], min_heap[parent])) {
			swap(min_heap[curr], min_heap[parent]);
			curr = parent;
			parent = (curr+1)/2-1;
		}
	}

public:

	PriorityQueue(const Cmp &comp = Cmp()) : cmp(comp) { }

	T extract_min(void) {
		T ret = min_heap[0];
		swap(min_heap[0], min_heap[min_heap.size()-1]);
		min_heap.pop_back();
		sift_down(0);
		return ret;
	}

	void insert(const T &elem) {
		min_heap.push_back(elem);
		sift_up(min_heap.size()-1);
	}

	size_type size(void) const {
		return min_heap.size();
	}

	bool empty(void) const {
		return min_heap.size() == 0;
	}
};

struct PatienceSortCmp {
	typedef vector<vector<int> >::size_type size_type;
	vector<vector<int> > *piles;

	bool operator()(size_type i, size_type j) {
		size_type pi_sz = (*piles)[i].size();
		size_type pj_sz = (*piles)[j].size();
		return (*piles)[i][pi_sz-1] < (*piles)[j][pj_sz-1];
	}
};

unsigned find_pile(const vector<vector<int> > &piles, int value) {
	int l = -1;
	int r = piles.size();

	while (l+1 != r) {
		int m = l+(r-l)/2;
		if (piles[m][piles[m].size()-1] <= value) {
			l = m;
		} else {
			r = m;
		}
	}

	return r;
}

void patience_sort(vector<int> &array) {
	vector<vector<int> > piles;

	for (vector<int>::size_type i = 0; i < array.size(); i++) {
		unsigned pile_idx = find_pile(piles, array[i]);
		if (pile_idx == piles.size()) {
			piles.push_back(vector<int>());
		}
		piles[pile_idx].push_back(array[i]);
	}

	PatienceSortCmp comp;
	comp.piles = &piles;
	PriorityQueue<vector<vector<int> >::size_type, PatienceSortCmp> queue(comp);
	for (vector<vector<int> >::size_type i = 0; i < piles.size(); i++) {
		queue.insert(i);
	}

	for (vector<int>::size_type i = 0; i < array.size(); i++) {
		vector<vector<int> >::size_type pile_i = queue.extract_min();
		array[i] = piles[pile_i][piles[pile_i].size()-1];
		piles[pile_i].pop_back();
		if (piles[pile_i].size() > 0) {
			queue.insert(pile_i);
		}
	}
}

int main(void) {
	cout << "Enter size of array, followed by the elements" << endl;
	cout << "> ";

	vector<int>::size_type arr_sz;
	while (cin >> arr_sz) {
		vector<int> array;

		for (vector<int>::size_type i = 0; i < arr_sz; i++) {
			int val;
			cin >> val;
			array.push_back(val);
		}

		patience_sort(array);

		cout << "[ " << array[0];
		for (vector<int>::size_type i = 1; i < arr_sz; i++) {
			cout << ", " << array[i];
		}
		cout << " ]" << endl;

		cout << "> ";
	}

	return 0;
}
