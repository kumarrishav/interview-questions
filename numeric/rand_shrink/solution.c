/* In the C standard library, rand() returns a random integer in the range [0..RAND_MAX]
 *
 * Let's assume that rand() is a very good pseudorandom generator; in particular, assume that it
 * returns any number in the range [0..RAND_MAX] with equal probability P = 1/(RAND_MAX+1)
 *
 * Write a function, range_rand(l, r), that uses rand() to shrink the original range size
 * by returning a random integer in the interval [l..r]. Assume l >= 0 and r <= RAND_MAX.
 *
 * What are our options? How can we do this while keeping a uniform probability distribution?
 * Discuss the different approaches and their limitations.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* This deserves a small explanation. First of all, there is really no bullet-proof solution.
 *
 * Let's focus on writing a function rand_zero(N) that generates random integers in the range
 * [0..N], where N <= RAND_MAX, since range_rand(l, r) is equivalent to rand_zero(r-l)+l.
 *
 * A traditional, straightforward and easy approach is to use the remainder operator. That is,
 * to select a random integer in [0..N], we simply do rand()%(N+1). This might be enough
 * for our purposes if we don't really care about keeping a uniform distribution.
 *
 * Many people know that using the remainder operator to shrink rand() ranges is bad because it
 * breaks the original distribution, but most programmers stop here - they don't really know why
 * this happens.
 *
 * The explanation is simple: by using remainders, we are essentially splitting the original numbers
 * in the range into buckets. It's fairly intuitive to see that when we try to pack a range [a..b]
 * into a smaller range [c..d] by splitting the b-a+1 integers in the former range around
 * d-c+1 buckets, unless d-c+1 evenly divides b-a+1, there will be buckets with more numbers
 * than the other buckets. Thus, there are certain values that will be more likely to appear
 * in a shrinked "random" sequence if we do this.
 *
 * Let's look at an example. Consider that RAND_MAX is 5. So, rand() picks any number from
 * the sequence [ 0, 1, 2, 3, 4, 5 ] with equal probability. Consider that we now want
 * to shrink that interval to [0..2], that is, we want random integers in the interval
 * [ 0, 1, 2 ]. Using the remainder means that we turn [ 0, 1, 2, 3, 4, 5 ] into
 * [ 0, 1, 2, 0, 1 ]. Because 3 doesn't evenly divide 5, the bucket for 0 and 1 has more
 * elements than the bucket for 2. This has the undesirable effect of breaking the distribution:
 * 0 is picked with probability 2/5, 1 is picked with probability 2/5, and 2 is picked with
 * probability 1/5. We don't have a uniform distribution anymore.
 *
 * How do we fix it? The key aspect is to keep in mind that we must always use the original range.
 * One possible approach is to keep generating random numbers until we get something in the desired
 * range:
 *
 * n = rand();
 * while (!(n >= l && n <= r)) {
 *     n = rand();
 * }
 *
 * Yes, it's simple and it works. It doesn't break the distribution. But it can loop for a long time
 * for small ranges. For example, what happens if RAND_MAX is 2^32-1 (as is often the case),
 * and the target range is [0..1]? We can be looking at a very long wait!
 *
 * Clearly, this is suboptimal, to say the least.
 *
 * Instead, we can do the following: find the largest value <= RAND_MAX that evenly divides the
 * desired range. Pick a random value. If it's greater than this value, try again, otherwise,
 * we have a uniform random value in a range that evenly divides the desired range, and so
 * we can use the remainder operator without affecting the distribution.
 *
 * Finding the largest value <= RAND_MAX that evenly divides the desired range [0..N] can be
 * done with:
 *
 * excess = (RAND_MAX+1)%(n+1)
 * limit = RAND_MAX-excess;
 *
 * Here, `limit+1` is the largest value that evenly divides `N+1`: first, we determine how many
 * excess elements we have that would affect the buckets distribution with (RAND_MAX+1)%(n-1).
 *
 * Then, we weed out those excess values by setting an upper limit.
 *
 * So, now we can just keep picking a random value. If it's greater than `limit`, try again, because
 * we don't want to select one of those values that is biased. Otherwise, we selected a value in a
 * range of length that evenly divides the desired range; we can just return that value modulo N+1.
 *
 * So, we get something like:
 *
 * excess = (RAND_MAX+1)%(n+1)
 * lim = RAND_MAX-excess
 * result = rand()
 * while (result > lim) {
 *     result = rand()
 * }
 * return result%(n+1)
 *
 * Now we just have to polish on an implementation details. RAND_MAX is often the maximum value
 * that can be represented by the type returned by rand(). So, we don't really want that
 * `(RAND_MAX+1)` in there wrapping around to 0. We can fix it by using the equivalent
 * form RAND_MAX%(n+1)+1.
 *
 * This is slightly better than blindly calling rand() until we're lucky enough to get a value
 * in range, but it's not a bullet proof solution. We can still loop for a long time, although
 * unlikely. But it can happen. Theoretically, we can loop forever, but that's very, very unlikely.
 *
 */

/* Returns a random integer in the range [0..max]
 * with the same distribution as stdlib's rand()
 *
 * `max` must be less than or equal to `RAND_MAX`
 */
int rand_zero(int max) {
	if (max > RAND_MAX) {
		return -1;
	} else if (max == RAND_MAX) {
		return rand();
	}

	int excess = RAND_MAX%(max+1)+1;
	int lim = RAND_MAX-excess;

	int ret;
	for (ret = rand(); ret > lim; ret = rand())
		; /* Intentionally left blank */

	return ret%(max+1);
}

/* Returns a random integer in the range [low..high]
 * with the same distribution as stdlib's rand()
 */
int range_rand(int low, int high) {
	if (high-low+1 > RAND_MAX) {
		return -1;
	}
	return rand_zero(high-low)+low;
}

int main(int argc, char *argv[]) {
	unsigned seed;

	if (argc > 1) {
		if (sscanf(argv[1], "%u", &seed) != 1) {
			fprintf(stderr, "Invalid seed \"%s\". Please use an integer >= 0.\n", argv[1]);
			exit(EXIT_FAILURE);
		}
	} else {
		seed = (unsigned) time(NULL);
	}

	srand(seed);

	printf("Enter a range [l, r], where l >= 0 and r <= %d, followed by the number of iterations\n", RAND_MAX);
	printf("> ");

	int l, h, i;
	while (scanf("%d%d%d", &l, &h, &i) == 3) {
		while (i--) {
			printf("%d\n", range_rand(l, h));
		}
		printf("> ");
	}

	return 0;
}
