# heapsort

Best case: O(n log(n))

Worst case: O(n log(n))

Average case: O(n log(n))

Memory: O(1)

Heapsort is pretty nice algorithm. It guarantees O(n log(n)) worst case time with O(1) memory usage, which is impressive. Heapsort can be seen as an enhancement of SelectionSort. Instead of naively running a linear scan through every element to find the next minimum, heapsort makes use of a *binary heap*, a fancy data structure that can always find the max. element in O(log(n)) time. In particular, heapsort uses a *max-heap*, which means that a node N is always greater than or equal to both of its children. Because a max-heap is used, heapsort iteratively finds the current known maximum and prepends it to the result - it sorts the array from right to left.

In practice, though, a well implemented quicksort is often faster than heapsort.
