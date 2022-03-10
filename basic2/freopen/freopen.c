#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
int main() {
	char *fname = "ssu_test.txt";
	int fd; //file descripter

	printf("First printf : Hello, OSLAB!!\n");

	if((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "oepn error for %s\n", fname);
		exit(1);
	}
	if(freopen(fname, "w", stdout) != NULL)
		printf("Second printf : Hello, OSLAB!!\n");
	exit(0);
}
