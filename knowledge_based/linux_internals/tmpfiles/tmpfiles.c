/* Explain the different methods to create temporary files, and how they differ */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* There are 3 ways to create a temporary file in linux:
 * - With tmpnam() followed by open()
 * - With tmpfile()
 * - With mkstemp()
 *
 * Let's look at the first approach.
 *
 * tmpnam() returns the name of a unique file that does not exist at the time of invocation.
 * A naive programmer is tempted to call tmpnam() followed by open() to create a temporary, unique
 * file.
 * However, this is not correct, because there is a time window between the call to tmpnam() and
 * the call to open() in which another process in the system could create
 * a file with the same name and start writing it - definitely an unfortunate and undesirable
 * situation. In other words, we want an atomic method of doing tmpnam() + open().
 *
 * This is made possible with the second approach. tmpfile() is equivalent to calling
 * open(tmpnam()), with the added benefit that it is atomic, so there are no race conditions,
 * or any other problems with interfering processes.
 * tmpfile() also calls unlink() on the created file, which ensures that the file created will
 * not be left around the system if the process terminates or crashes.
 *
 * The third approach uses mkstemp(). mkstemp() has slightly different semantics: first, we can
 * pass it a template string (which should be terminated with XXXXXX) to be used in the filename
 * generator.
 * The filename generation and its creation are also atomic, but mkstemp() does not call unlink on
 * the file. We are responsible for closing and unlinking it.
 *
 */

void tmpnam_and_open(void) {
	printf("Entered tmpnam_and_open()\n");
	char *filename = tmpnam(NULL);
	printf("filename = %s\n", filename);

	int fd;
	fd = open(filename, O_RDWR | O_CREAT, 0);

	if (fd < 0) {
		perror("Error creating file");
	} else {
		printf("Created file %s\n", filename);
		unlink(filename);
	}

	printf("Leaving tmpnam_and_open()\n");
}

void tmpfile_method(void) {
	printf("Entered tmpfile_method()\n");

	FILE *tmp_file = tmpfile();
	if (tmp_file == NULL) {
		perror("Couldn't create temporary file");
	} else {
		printf("Created temporary file\n");
	}

	printf("Leaving tmpfile_method()\n");
}

void mkstemp_method(void) {
	printf("Entered mkstemp_method()\n");

	static char template_buff[] = "mytemplateXXXXXX";

	int fd;
	fd = mkstemp(template_buff);
	if (fd < 0) {
		perror("Couldn't create temporary file");
	} else {
		printf("Created temporary file %s\n", template_buff);
		unlink(template_buff);
	}

	printf("Leaving mkstemp_method()\n");
}

int main(void) {
	tmpnam_and_open();
	tmpfile_method();
	mkstemp_method();
	getchar();
	return 0;
}
