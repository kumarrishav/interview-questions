# quicksort

Best case: O(n log(n))

Worst case: O(n^2)

Average case: O(n log(n))

Memory: O(log(n))

Ahh... quicksort! Quicksort is a classic sorting algorithm. It is very well known, and it is commonly the algorithm of choice in sorting libraries. Yes, theoretically, it can degrade to O(n^2), but that is extremely unlikely to happen, especially if pivots are chosen randomly. It is relatively easy to code, doesn't need much memory, and in practice, as a general purpose algorithm, it beats most of the competition.

Quicksort's core operation is *partitioning*. At each step, a pivot value is chosen. The array is then partitioned in such a way that all the elements less than the pivot are to its left, and all elements greater than the pivot are to its right. When that is the case, the pivot is now in its final position. The left subarray and the right subarray are sorted recursively.

Even though quicksort is traditionally implemented recursively (that's the most natural way to do it), iterative implementations are also possible (and not excruciatingly hard to understand). [See the discussion about Exercise 2-1 of *The Practice of Programming*](https://github.com/filipegoncalves/tpop/tree/master/chapter2/ex2-1).
