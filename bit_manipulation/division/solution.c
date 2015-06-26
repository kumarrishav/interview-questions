/* An implementation of the traditional long division algorithm for binary numbers.
 * Knowing how division works is usually not required for the purposes of an interview,
 * but it is helpful for other important topics. For example, the long division algorithm
 * plays a crucial role in CRCs (Cyclic Redundancy Checks), a technique often used by
 * link layer network protocols to detect errors in transmitted frames.
 *
 * This algorithm is also known as shift and subtract. Some interviewers like to see candidates
 * implement the division algorithm, but it's not a very common question.
 *
 * Nevertheless, it is always a good idea to master bit manipulation algorithms.
 *
 * We will start by showing long division in action with an example. It works pretty much the
 * same way as base-10 long division, except that it's easier! Why don't kids learn this
 * in primary school? Tsk tsk...
 *
 * Let's calculate 11011011/10110 (219/22):
 *
 *        ___________
 * 10110 ) 11011011
 *
 * We start scanning the bits of the dividend from most significant to least significant until
 * the number seen is greater than or equal to the divisor. Until this happens, we append 0s to
 * the result.
 * So, for example, we see a 1; 1 is less than 10110, so we scan to the next bit. Now we have 11.
 * 11 is still less than 10110, so we move to the next bit. Now we have 110. 110 is less than 10110,
 * so we move to the next bit. This keeps happening until we have 11011. When the current number is
 * greater than or equal to the divisor, we append 1 to the result, subtract the current number from
 * the dividend, and borrow the next bit in the dividend:
 *
 *        _____1_____
 * 10110 ) 11011011
 *         10110
 *        ------
 *         001010
 *
 * Now the current number is 1010, and we repeat the same algorithm. 1010 < 10110, so we append 0
 * to the result and borrow the next bit in the dividend:
 *
 *        _____10____
 * 10110 ) 11011011
 *         10110
 *        ------
 *         0010101
 *
 * 10101 < 10110, so again we append a 0 and borrow the next bit:
 *
 *        _____100___
 * 10110 ) 11011011
 *         10110
 *        ------
 *         00101011
 *
 * 101011 >= 10110, so we append 1 to the result, subtract and borrow - but this time we don't have
 * anything to borrow, so the algorithm ends
 *
 *        _____1001__
 * 10110 ) 11011011
 *         10110
 *        ------
 *         00101011
 *            10110
 *         --------
 *           010101
 *
 * So 11011011/10110 = 1001 with remainder 10101 (219/22 = 9 with remainder 21)
 *
 * This is implemented by keeping a mask that selects the next bit in the dividend. The mask is
 * used to select the bit, and then that bit is appended to the current dividend (divd in the code)
 * with divd = (divd << 1) | !! (dividend & mask)
 *
 * The mask is initialized to all 0s except for the most significant bit. To remain platform
 * independent, this is done with ~(~0U >> 1U).
 * For example, in a 32-bit platform ~0U is 11111111111111111111111111111111.
 * So, ~0U >> 1U is 01111111111111111111111111111111. Then, we negate it again to get
 * 10000000000000000000000000000000 and we start from there. At each step in the loop,
 * we select the bit from the dividend with the mask, append it to the current dividend (divd),
 * and compare that number with the divisor. Then we just follow the algorithm: if divd is less
 * than the divisor, we append a 0 to the result, update the mask, and repeat.
 * If divd is greater than or equal to the divisor, we subtract it, append a 1 to the result,
 * and update the mask.
 *
 * The algorithm was also extended to handle signed division. This is trivial: the main algorithm
 * always performs unsigned division, and adjusts the sign of the result as a final step.
 *
 * Note that this approach yields integer division that rounds towards 0. ANSI C does not specify
 * the behavior of signed division: it is up to the implementation to decide whether signed division
 * is rounded towards 0 or towards negative infinity.
 *
 * It is also assumed that the division being performed is legal and will not overflow.
 *
 * It is often a good idea to identify the limitations of our code and identify shady edge cases.
 * On a system that uses twos complement representation, this algorithm will overflow whenever
 * the dividend is INT_MIN. This happens because the algorithm always converts negative integers to
 * positive before performing any computation, and abs(INT_MIN) is not representable.
 *
 * One possible solution is to use a wider type to perform intermediate computation and then convert
 * the final result back to int.
 */

#include <stdio.h>

int divide(int dividend, int divisor) {
	int res = 0;
	unsigned mask = ~(~0U >> 1U);
	int divd = 0;

	int sign = ((dividend < 0) != (divisor < 0)) ? -1 : 1;
	if (dividend < 0) {
		dividend = -dividend;
	}
	if (divisor < 0) {
		divisor = -divisor;
	}

	while (mask != 0) {
		divd = (divd << 1) | !!(dividend & mask);
		if (divd < divisor) {
			res <<= 1;
		} else {
			res = (res << 1) | 1;
			divd -= divisor;
		}
		mask >>= 1U;
	}

	return res*sign;
}

int main(void) {
	printf("Enter dividend and divisor\n");
	printf("> ");

	int dividend, divisor;
	while (scanf("%d%d", &dividend, &divisor) == 2) {
		printf("%d\n", divide(dividend, divisor));
		printf("> ");
	}

	return 0;
}
