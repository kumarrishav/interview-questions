
#include <sys/utsname.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {

	struct utsname uname_buf;
	if (uname(&uname_buf) < 0) {
		perror("Couldn't get system information");
		exit(EXIT_FAILURE);
	}

	printf("System name: %s\n", uname_buf.sysname);
	printf("Node name: %s\n", uname_buf.nodename);
	printf("Release: %s\n", uname_buf.release);
	printf("Version: %s\n", uname_buf.version);
	printf("Machine: %s\n", uname_buf.machine);

	#ifdef _GNU_SOURCE
	printf("Domain name: %s\n", uname_buf.domainname);
	#endif

	return 0;
}
