# countingsort

Let `k` be the size of the set with all the possible input values

Best case: O(n+k)

Worst case: O(n+k)

Average case: O(n+k)

Memory: O(k)

Whenever you can, use counting sort. Counting sort achieves the ultimate performance gain by bringing us O(n) sorting. Sadly, it's unfeasible for large input spaces, such as 32-bit integers. It works by counting the number of times each key of the input space appeared. To do that, it needs O(k) memory, and because the algorithm scans through the input and then must scan through the auxiliary memory used for counting, it takes O(n+k) time. Since this is mostly useful (and usable) for small input spaces, it is common to say that counting sort is linear in the size of the input.

For situations where the input contains at most one element for each possible value, counting sort can be implemented with a bit vector. This is the approach taken in the sample implementation. The sample implementation is based off the problem in Column 1 of *Programming Pearls* - sort 10-million integers, where each integer is 7 digits, using roughly 1 MB of memory. No data is associated with the integers, and there are no duplicates. Bit-vectors to the rescue!
