
package main

import "fmt"

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
