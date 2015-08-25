/* A frog only moves forward, but it can move in steps 1 inch long or in jumps 2 inches long. A frog
 * can cover the same distance using different combinations of steps and jumps.
 *
 * Write a function that calculates the number of different combinations a frog can use to cover a
 * given distance.
 *
 * For example, a distance of 3 inches can be covered in three ways: step-step-step, step-jump,
 * and jump-step.
 *
 * Source: StackOverflow
 */

package main

/* This is a nice solution in Go, as an alternative to C. */

import "fmt"

/* This problem is reduced to the problem of finding the N-th Fibonacci number, which can be solved
 * in O(N) time and O(1) memory
 *
 * See http://stackoverflow.com/questions/32202911
 */
func fib() func() int {
	before_last, last := 1, 0
	return func() int {
		last, before_last = before_last+last, last
		return last
	}
}

func frog_combos(n int) int {
	f := fib()
	for i := 0; i < n; i++ {
		f()
	}
	return f()
}

func main() {
	for i := 0; i < 10; i++ {
		fmt.Printf("frog_combos(%v) = %v\n", i, frog_combos(i))
	}
}
