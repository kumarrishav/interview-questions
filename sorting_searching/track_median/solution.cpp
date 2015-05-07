/* Numbers are randomly generated and stored into an (expanding) array. How
 * would you keep track of the median?
 *
 * Source: Cracking the Coding Interview, page 54 (Example question in the section "Approach V")
 */
#include <iostream>
#include <vector>

using namespace std;

class MinHeapComparator {
public:
	bool operator()(int n1, int n2) {
		return n1 > n2;
	}
};

class MaxHeapComparator {
public:
	bool operator()(int n1, int n2) {
		return n1 < n2;
	}
};

// Max-heap according to the comparator
template <class Compare>
class Heap {
private:
	vector<int> heap;
	Compare comparator;

	void swap(vector<int>::size_type i, vector<int>::size_type j) {
		int tmp = heap[i];
		heap[i] = heap[j];
		heap[j] = tmp;
	}

public:
	typedef vector<int>::size_type size_type;

	size_type size(void) { return heap.size(); }

	int peek(void) { return heap[0]; }

	void insert(int v) {
		heap.push_back(v);

		if (heap.size() == 1) {
			return;
		}

		size_type parent = heap.size()/2-1;
		size_type curr = heap.size()-1;

		while (parent > 0 && comparator(heap[parent], heap[curr])) {
			swap(parent, curr);
			curr = parent;
			parent = (parent-1)/2;
		}

		if (comparator(heap[parent], heap[curr])) {
			swap(parent, curr);
		}
	}

	int extract(void) {
		int popped = heap[0];
		int last = heap[heap.size()-1];

		heap.pop_back();

		if (heap.size() == 0) {
			return popped;
		}

		heap[0] = last;

		size_type curr = 0;
		size_type left = (curr+1)*2-1;
		size_type right = left+1;
		size_type largest = curr;

		if (left < heap.size() && comparator(heap[largest], heap[left])) {
			largest = left;
		}
		if (right < heap.size() && comparator(heap[largest], heap[right])) {
			largest = right;
		}

		while (curr != largest) {
			swap(curr, largest);

			curr = largest;
			left = (curr+1)*2-1;
			right = left+1;

			if (left < heap.size() && comparator(heap[largest], heap[left])) {
				largest = left;
			}
			if (right < heap.size() && comparator(heap[largest], heap[right])) {
				largest = right;
			}
		}

		return popped;
	}
};

class MedianTracker {
private:
	Heap<MaxHeapComparator> heap_l;
	Heap<MinHeapComparator> heap_r;

	void balance_heaps(unsigned size_l, unsigned size_r) {
		while (heap_l.size() != size_l && heap_r.size() != size_r) {
			if (heap_l.size() > size_l) {
				heap_r.insert(heap_l.extract());
			} else {
				heap_l.insert(heap_r.extract());
			}
		}
	}

public:
	double median(void) {
		if ((heap_l.size()+heap_r.size()) & 1) {
			return heap_l.peek();
		} else {
			return (heap_l.peek()+heap_r.peek()*1.0)/2.0;
		}
	}

	void insert(int n) {
		unsigned elements = heap_l.size()+heap_r.size();

		if (elements == 0 || n <= heap_l.peek()) {
			heap_l.insert(n);
		} else {
			heap_r.insert(n);
		}

		elements++;

		unsigned size_l = elements/2;
		unsigned size_r = size_l;
		if (elements & 1) {
			size_l++;
		}

		balance_heaps(size_l, size_r);
	}
};

int main(void) {
	cout << "The available commands are:" << endl;
	cout << "I n - Insert the value `n` into the stream" << endl;
	cout << "M - Get the median" << endl;
	cout << "Q - Quit" << endl;
	cout << "> ";

	MedianTracker tracker;

	string op;
	while (cin >> op) {
		if (op == "I") {
			int val;
			cin >> val;
			tracker.insert(val);
			cout << "Inserted " << val << " into the median" << endl;
		} else if (op == "M") {
			cout << "Median = " << tracker.median() << endl;
		} else if (op == "Q") {
			break;
		} else {
			cout << "Unrecognized command: " << op;
		}
		cout << "> ";
	}

	return 0;
}
