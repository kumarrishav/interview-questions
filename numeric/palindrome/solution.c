/* Write a function to check if an integer is a palindrome.
 *
 * An integer is considered to be a palindrome if the digits composing it
 * in base-10 form a palindrome.
 *
 * Negative integers are never palindromes.
 *
 * EXAMPLE
 *
 * 123321 is a palindrome
 * 123421 is not a palindrome
 *
 */
#include <stdio.h>

int is_palindrome(int n) {
    
    if (n < 0) {
        return 0;
    }
    
    unsigned long long int divn;
    unsigned long long int rev;
    
    for (rev = 0, divn = 1; n/divn != 0; divn *= 10) {
        rev = rev*10+(n/divn)%10;
    }

    return rev == n;
}

int main(void) {
	printf("Enter a number to see if it's a palindrome.\n");
	printf("> ");

	int v;
	while (scanf("%d", &v) == 1) {
		if (is_palindrome(v)) {
			printf("Yes.\n");
		} else {
			printf("No.\n");
		}
		printf("> ");
	}

	return 0;
}
