# patience sort

Best case: O(n log(n))

Worst case: O(n log(n))

Average case: O(n log(n))

Memory: O(n)

Patience sort is a sorting algorithm based on a solitaire card game. The algorithm is very similar to heapsort, but slightly more complex to implement, and so it is not much used in practice. In fact, as a sorting algorithm per se, it is not very interesting. However, it is very useful because of the way it structures data while sorting it. For example, a modified version of Patience Sorting can be used to find the Longest Increasing Subsequence of an array in O(N log(N)) time with a small modification. That's what makes this algorithm interesting and worthwhile to study.

This example taken from Wikipedia shows how the algorithm works to sort a shuffled deck of cards. In this example, sorting is seen as a game. The principle is the same for arbitrarily large arrays:

The game begins with a shuffled deck of cards, labeled 1, 2, ..., n.

The cards are dealt one by one into a sequence of piles on the table, according to the following rules:

- Initially, there are no piles. The first card dealt forms a new pile consisting of the single card.
- Each new card may be placed either on an existing pile whose top card has a value higher than the new card's value, thus increasing the number of cards in that pile, or to the right of all of the existing piles, thus forming a new pile.
- When there are no more cards remaining to deal, the game ends.

Note that at each step, the top elements in each pile form an increasing sequence from left to right. To obtain the sorted array, find the smallest element among all the elements in the top of each pile, write it to the output array, and pop it out of the pile. Repeat this until every element has been processed. To efficiently find the smallest element among all elements in the top of each pile, use a priority queue with O(log(N)) extract-min. This can be accomplished with a min heap.

With an efficient priority queue implementation, the overall complexity is O(N log(N)).

## Note
To see the O(N log(N)) algorithm for the Longest Increasing Subsequence using Patience Sorting, see [DS_Algos/LIS/optimized1.cpp](https://github.com/filipegoncalves/interview-questions/blob/master/DS_Algos/LIS/optimized1.cpp).
