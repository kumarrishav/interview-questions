/* A heap-based priority queue. Higher priorities correspond to lower priority values.
 * Thus, a min-heap is used.
 *
 * enqueue: O(log(N))
 * dequeue: O(log(N))
 * peek: O(1)
 *
 */
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

template<class T>
class PriorityQueue {
private:

	struct PriorityQueueNode {
		int value;
		T obj;
	};

	vector<PriorityQueueNode> heap;

	void swap(typename vector<PriorityQueueNode >::size_type i,
		  typename vector<PriorityQueueNode >::size_type j) {
		PriorityQueueNode tmp = heap[i];
		heap[i] = heap[j];
		heap[j] = tmp;
	}

	void sift_up(typename vector<PriorityQueueNode >::size_type i) {
		while (i != 0 && heap[i].value < heap[i/2].value) {
			swap(i, i/2);
			i /= 2;
		}
	}

	void sift_down(typename vector<PriorityQueueNode >::size_type i) {
		typename vector<PriorityQueueNode >::size_type smallest = i;
		typename vector<PriorityQueueNode >::size_type left = 2*i+1;
		typename vector<PriorityQueueNode >::size_type right = left+1;

		if (left < heap.size() && heap[left].value < heap[smallest].value) {
			smallest = left;
		}
		if (right < heap.size() && heap[right].value < heap[smallest].value) {
			smallest = right;
		}

		while (smallest != i) {
			swap(i, smallest);
			i = smallest;
			left = 2*i+1;
			right = left+1;

			if (left < heap.size() && heap[left].value < heap[smallest].value) {
				smallest = left;
			}
			if (right < heap.size() && heap[right].value < heap[smallest].value) {
				smallest = right;
			}
		}
	}

public:
	PriorityQueue(void) { }

	void enqueue(int val, T obj) {
		PriorityQueueNode newnode;
		newnode.value = val;
		newnode.obj = obj;
		heap.push_back(newnode);
		sift_up(heap.size()-1);
	}

	int dequeue(T &out) {
		assert(heap.size() > 0);
		PriorityQueueNode extracted = heap[0];
		heap[0] = heap[heap.size()-1];
		heap.pop_back();

		if (heap.size() > 0) {
			sift_down(0);
		}

		out = extracted.obj;
		return extracted.value;
	}

	int peek(T &out) {
		assert(heap.size() > 0);
		out = heap[0].obj;
		return heap[0].value;
	}
};

int main(void) {
	cout << "Commands available:" << endl;
	cout << "E p v - enqueue an element with priority p and value v" << endl;
	cout << "D - dequeue" << endl;
	cout << "P - peek" << endl;
	cout << "Q - quit" << endl;
	cout << "> ";

	PriorityQueue<int> queue;

	char op;
	while (cin >> op) {
		if (op == 'E') {
			int priority, val;
			cin >> priority >> val;
			queue.enqueue(priority, val);
		} else if (op == 'D') {
			int res;
			int prio = queue.dequeue(res);
			cout << "Dequeued element " << res << " with priority " << prio << endl;
		} else if (op == 'P') {
			int res;
			int prio = queue.peek(res);
			cout << "Head contains element " << res << " with priority " << prio << endl;
		} else if (op == 'Q') {
			break;
		} else {
			cerr << "Unknown option: " << op;
		}
		cout << "> ";
	}

	return 0;
}
