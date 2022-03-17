#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	char *fname = "ssu_test.txt";
	int fd;

	if((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error for %s", fname);
		exit(1);
	}
	else {
		close(fd);
		fd = open(fname, O_RDWR);
		printf("Success!\n<%s> is new readable and writable\n", fname);
	}
	exit(0);
}
