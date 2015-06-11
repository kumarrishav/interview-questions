/* Explain how users are typically authenticated in a modern Linux system.
 * Demonstrate your ideas by implementing a bare-bones login program that
 * prompts the user for his password and checks if the password matches with
 * the stored password entry for that user.
 *
 * Source: Adapted from Advanced Programming in the UNIX Environment, 3rd Edition
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>
#include <assert.h>

/* NOTES:
 *
 * - Compile with -lcrypt
 *
 * - Either run as root or setgid root (to be able to read /etc/shadow)
 *
 * - The code assumes that crypt(3) is implemented and working
 *
 * - The code assumes that shadow passwords are stored according to glibc2 additional encryption
 *   format described in the manpage:
 *   Glibc notes
 *     The glibc2 version of this function supports additional encryption algorithms.
 *
 *     If salt is a character string starting with the characters "$id$" followed by a string terminated by "$":
 *
 *            $id$salt$encrypted
 *
 *     then  instead  of  using the DES machine, id identifies the encryption method used and this then determines how the rest of the password
 *     string is interpreted.  The following values of id are supported:
 *
 *            ID  | Method
 *            ─────────────────────────────────────────────────────────
 *            1   | MD5
 *            2a  | Blowfish (not in mainline glibc; added in some
 *                | Linux distributions)
 *            5   | SHA-256 (since glibc 2.7)
 *            6   | SHA-512 (since glibc 2.7)
 *
 *     So $5$salt$encrypted is an SHA-256 encoded password and $6$salt$encrypted is an SHA-512 encoded one.
 *
 *     "salt" stands for the up to 16 characters following "$id$" in the salt.  The encrypted part of the password string is  the  actual  com‐
 *     puted password.  The size of this string is fixed:
 *
 *     MD5     | 22 characters
 *     SHA-256 | 43 characters
 *     SHA-512 | 86 characters
 *
 *     The  characters  in  "salt"  and "encrypted" are drawn from the set [a–zA–Z0–9./].  In the MD5 and SHA implementations the entire key is
 *     significant (instead of only the first 8 bytes in DES).
 *
 * EXAMPLE
 *
 * This password entry in /etc/shadow:
 *
 * $6$qAGwHUmc$oztSpKV/0t97zOBedwSrS1BOCVm3d/Oj2EyQHYqlDrgCB2Kr1TpAtyPDnWJ1MfksLdRdiS1FDiBC.luOSYbSE.
 *
 * Is generated with:
 *
 * crypt(pass, "$6$qAGwHUmc$");
 *
 */

/* We use our own readpass function instead of getpass(3) because the manpage mentions it's obsolete */
int readpass(char *password, size_t len, const char *prompt) {
	int ret = 0;
	int term = open("/dev/tty", O_RDWR);
	if (term < 0) {
		return -1;
	}

	struct termios oldf, newf;
	if (tcgetattr(term, &oldf) < 0) {
		ret = -1;
		goto clean_close;
	}

	newf = oldf;
	newf.c_lflag &= ~ECHO;
	newf.c_lflag |= ECHONL;

	if (tcsetattr(term, TCSANOW, &newf) < 0) {
		ret = -1;
		goto clean_close;
	}

	size_t prompt_sz = strlen(prompt);
	if (write(term, prompt, prompt_sz) != prompt_sz) {
		ret = -1;
		goto clean_attr;
	}

	int n;
	if ((n = read(term, password, len-1)) < 0) {
		ret = -1;
		goto clean_attr;
	}

	password[n] = '\0';

	if (n > 0 && password[n-1] == '\n') {
		password[n-1] = '\0';
	}

clean_attr:
	ret = tcsetattr(term, TCSANOW, &oldf);
clean_close:
	close(term);
	return ret;
}

int pass_correct(struct spwd *spentry, const char *pass) {
	char *encrypted = spentry->sp_pwdp;
	size_t separators = 0;
	size_t salt_end = 0;

	while (encrypted[salt_end] != '\0' && separators < 3) {
		if (encrypted[salt_end] == '$') {
			separators++;
		}
		salt_end++;
	}

	if (separators < 3) {
		return -1;
	}

	char tmp = encrypted[salt_end];
	encrypted[salt_end] = '\0';

	char *input_crypt = crypt(pass, encrypted);
	if (input_crypt == NULL) {
		return -1;
	}

	encrypted[salt_end] = tmp;

	return !strcmp(encrypted, input_crypt);
}

int main(void) {
	static char password_buf[1024];

	if (readpass(password_buf, sizeof(password_buf), "Password: ") < 0) {
		perror("Couldn't read password from terminal");
		exit(EXIT_FAILURE);
	}

	char *username = getlogin();
	if (username == NULL) {
		perror("Couldn't get login username");
		exit(EXIT_FAILURE);
	}

	struct spwd *spentry = getspnam(username);
	if (spentry == NULL) {
		fprintf(stderr, "Couldn't get password information for %s: %s\n", username, strerror(errno));
		exit(EXIT_FAILURE);
	}

	int res = pass_correct(spentry, password_buf);

	if (res == -1) {
		fprintf(stderr, "Internal error. Can't validate password.\n");
	} else if (res == 1) {
		printf("Password accepted.\n");
	} else if (res == 0) {
		printf("Access denied.\n");
	} else {
		assert(0);
	}

	return 0;
}
