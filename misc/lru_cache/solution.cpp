
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

		entry_it.set_value(val);
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
	LRUCache<uintptr_t, int> cache(16, 4);

	cache.insert(4, 1);
	cache.insert(8, 2);
	cache.insert(16, 3);
	cache.insert(32, 4);

	int myval;
	if (cache.get(4, myval)) {
		cout << "Accessed location 4, value = " << myval << endl;
	} else {
		cout << "Oops! Couldn't find address 4 in cache" << endl;
	}

	cache.insert(64, 5);

	if (!cache.get(8, myval)) {
		cout << "8 has been removed from cache" << endl;
	} else {
		cout << "Oops! 8 is still in the cache!" << endl;
	}

	cout << cache;

	return 0;
}
