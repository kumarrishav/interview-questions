/* Describe how a cache is structured and how it works.
 *
 * Discuss the various replacement strategies. Design and implement a
 * program that illustrates the cache interface and how it works inside.
 *
 * For the purposes of your program, assume that the replacement strategy
 * is LRU (Least Recently Used). How can you implement an LRU cache efficiently?
 *
 * Source: Careercup (Amazon interview)
 */
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdint>

using namespace std;

typedef unsigned long long timestamp_t;

template <class Key, class Value>
class CacheLineEntry {
private:
	timestamp_t timestamp;
	Key key;
	Value val;
public:
	CacheLineEntry(timestamp_t time, const Key &k, const Value &value) :
		timestamp(time), key(k), val(value) { }

	timestamp_t get_timestamp(void) const { return timestamp; }
	const Value &get_value(void) const { return val; }
	const Key &get_key(void) const { return key; }

	void set_timestamp(timestamp_t new_time) { timestamp = new_time; }
	void set_value(const Value &newvalue) { val = newvalue; }

	bool operator<(const CacheLineEntry<Key, Value> &rhs) const {
		return timestamp < rhs.timestamp;
	}
};

template <class Key, class Value>
class CacheLine {
public:
	typedef typename vector<CacheLineEntry<Key, Value> >::size_type size_type;
private:
	vector<CacheLineEntry<Key, Value> > min_heap;
	size_type assoc;

	typename vector<CacheLineEntry<Key, Value> >::iterator find_by_key(const Key &k) {
		for (typename vector<CacheLineEntry<Key, Value> >::iterator it = min_heap.begin();
		     it != min_heap.end();
		     it++) {
			if (it->get_key() == k) {
				return it;
			}
		}
		return min_heap.end();
	}

	void sift_up(size_type curr) {
		size_type parent = (curr+1)/2-1;
		while (curr != 0 && min_heap[curr] < min_heap[parent]) {
			swap(min_heap[curr], min_heap[parent]);
			curr = parent;
			parent = (curr+1)/2-1;
		}
	}

	void sift_down(size_type curr) {
		size_type left = curr*2+1;
		size_type right = left+1;
		size_type smallest = curr;

		if (left < min_heap.size() && min_heap[left] < min_heap[smallest]) {
			smallest = left;
		}
		if (right < min_heap.size() && min_heap[right] < min_heap[smallest]) {
			smallest = right;
		}

		while (smallest != curr) {
			swap(min_heap[curr], min_heap[smallest]);

			curr = smallest;
			left = curr*2+1;
			right = left+1;

			if (left < min_heap.size() && min_heap[left] < min_heap[smallest]) {
				smallest = left;
			}
			if (right < min_heap.size() && min_heap[right] < min_heap[smallest]) {
				smallest = right;
			}

		}
	}

	void extract_min(void) {
		swap(min_heap[0], min_heap[min_heap.size()-1]);
		min_heap.pop_back();
		sift_down(0);
	}

	void insert(const CacheLineEntry<Key, Value> &elem) {
		min_heap.push_back(elem);
		sift_up(min_heap.size()-1);
	}

	void increase_key(size_type idx, timestamp_t new_key) {
		min_heap[idx].set_timestamp(new_key);
		sift_down(idx);
	}

public:

	CacheLine(size_type associativity) : assoc(associativity) { }

	void insert(timestamp_t curr_time, const Key &k, const Value &val) {
		if (min_heap.size() == assoc) {
			extract_min();
		}
		insert(CacheLineEntry<Key, Value>(curr_time, k, val));
	}

	bool get(timestamp_t curr_time, const Key &k, Value &res) {
		typename vector<CacheLineEntry<Key, Value> >::iterator entry_it = find_by_key(k);

		if (entry_it == min_heap.end()) {
			return false;
		}

		res = entry_it->get_value();
		increase_key(entry_it-min_heap.begin(), curr_time);

		return true;
	}

	void update(timestamp_t curr_time, const Key &k, const Value &val) {
		typename vector<CacheLineEntry<Key, Value> >::iterator entry_it = find_by_key(k);

		if (entry_it == min_heap.end()) {
			return;
		}

		entry_it->set_value(val);
		increase_key(entry_it-min_heap.begin(), curr_time);
	}

	template <class K, class V>
	friend ostream &operator << (ostream &os, const CacheLine<K, V> &cline);

};

template <class Key, class Value>
ostream &operator << (ostream &os, const CacheLine<Key, Value> &cline) {
	for (typename vector<CacheLineEntry<Key, Value> >::const_iterator entry_it = cline.min_heap.begin();
	     entry_it != cline.min_heap.end();
	     entry_it++) {
		os << "| (" << entry_it->get_key() << " -> " << entry_it->get_value() << ") ";
	}
	return os;
}

template <class Key, class Value>
class LRUCache {
public:
	typedef typename vector<CacheLine<Key, Value> >::size_type size_type;
private:
	vector<CacheLine<Key, Value> > lines;
	size_type lines_count;
	size_type assoc;
	unsigned long long timestamp;
public:
	LRUCache(size_type blocks, size_type associativity) :
		lines_count(blocks/associativity), assoc(associativity), timestamp(0)  {
		lines.resize(lines_count, CacheLine<Key, Value>(assoc));
	}

	void insert(const Key &k, const Value &val) {
		lines[k%lines_count].insert(timestamp++, k, val);
	}

	bool get(const Key &k, Value &out) {
		bool ret = lines[k%lines_count].get(timestamp, k, out);
		if (ret) {
			timestamp++;
		}
		return ret;
	}

	void update(const Key &k, const Value &val) {
		lines[k%lines_count].update(timestamp++, k, val);
	}

	template <class K, class V>
	friend ostream &operator << (ostream &os, const LRUCache<K, V> &cache);
};

template <class Key, class Value>
ostream &operator << (ostream &os, const LRUCache<Key, Value> &cache) {

	for (typename vector<CacheLine<Key, Value> >::const_iterator line_it = cache.lines.begin();
	     line_it != cache.lines.end();
	     line_it++) {
		os << line_it-cache.lines.begin() << ":\t" << *line_it << endl;
	}

	return os;
}

int main(void) {
	cout << "Cache size in blocks: ";
	LRUCache<uintptr_t, int>::size_type blocks;
	cin >> blocks;
	cout << "Cache associativity: ";
	LRUCache<uintptr_t, int>::size_type assoc;
	cin >> assoc;

	LRUCache<uintptr_t, int> cache(blocks, assoc);

	cout << "*** LRU Cache of " << blocks << " blocks (" << assoc << "-way set associative) created" << endl;
	cout << "Commands available:" << endl;
	cout << "I ptr value - cache the memory location `ptr` storing the value `value`" << endl;
	cout << "U ptr value - update the already cached memory location `ptr` to store the value `value`" << endl;
	cout << "G ptr - find out if memory location `ptr` is cached, and if it is, return the value stored" << endl;
	cout << "P - print cache contents" << endl;
	cout << "Q - quit" << endl;
	cout << "Note: Please use integer values only. Memory locations should be entered using hexadecimal notation" << endl << endl;
	cout << "> ";

	char op;
	while (cin >> op) {
		if (op == 'I') {
			void *address;
			int value;
			cin >> address;
			cin >> value;
			cache.insert((uintptr_t) address, value);
			cout << "Inserted: (" << address << " -> " << value << ")" << endl;
		} else if (op == 'U') {
			void *address;
			int newval;
			cin >> address;
			cin >> newval;
			cache.update((uintptr_t) address, newval);
			cout << "Updated " << address << " value to " << newval << endl;
		} else if (op == 'G') {
			bool res;
			int val;
			void *address;
			cin >> address;
			res = cache.get((uintptr_t) address, val);
			if (res) {
				cout << "(" << address << " -> " << val << ")" << endl;
			} else {
				cout << address << " is not cached." << endl;
			}
		} else if (op == 'P') {
			cout << cache;
		} else if (op == 'Q') {
			break;
		} else {
			cerr << "Unrecognized command: " << op << endl;
		}
		cout << "> ";
	}

	return 0;
}
