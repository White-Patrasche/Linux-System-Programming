#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#define INPUT_SIZE 4096
void find();
void exit();
void help();

void find_fuc(char* INPUT) {
	struct stat* buf;
	printf("Index Size Mode      Blocks Links UID  GID  Access    Change         Modify         Path\n");
	stat("/Linux-System-Programming/struct1/a", buf);
	printf("I-node : %ld\n",(long) buf->st_ino);
	printf("size : %lld\n",(long long) buf->st_size);
	printf("Block : %lld\n",(long long) buf->st_blocks);
	printf("File Size : %lld\n", (long long) buf->st_size);
	printf("%s\n",ctime(&buf->st_atime));
//	printf("%s\n",ctime(&buf->st_ctime));
//	printf("%s\n",ctime(&buf->st_mtime));
	return;
}

void inputClear(char* INPUT) {
	for(int i=0; i<100; i++) INPUT = 0;
}
void help() {
	printf("Usage:\n");
	printf("\t> find [FILENAME] [PATH]\n");
	printf("\t\t>> [INDEX] [OPTION ... ]\n");
	printf("\t> help\n");
	printf("\t> exit\n\n");
	printf("\t[OPTION ... ]\n");
	printf("\t q : report only when files differ\n");
	printf("\t s : report when two files are the same\n");
	printf("\t i : ignore case differences in file contents\n");
	printf("\t r : recursively compare any subdirectories found\n");
	return;
}

int main() {
	char input[INPUT_SIZE];
	memset(input, 0, sizeof(input));
	struct timeval start_time; //time check
	gettimeofday(&start_time, NULL);
	while(1) {
		printf("20193010> ");
		fgets(input,INPUT_SIZE,stdin); //input includes '\n'
		if(strcmp(input, "find\n") == 0) { //input == find
			find_fuc(input);
		}
		else if(strcmp(input, "exit\n") == 0) { //input == exit
			struct timeval end_time;
			gettimeofday(&end_time, NULL);
			long SEC = end_time.tv_sec - start_time.tv_sec;
			long USEC = end_time.tv_usec - start_time.tv_usec;
			if(end_time.tv_usec - start_time.tv_usec < 0) { //if end usec is smaller
				SEC--;
				USEC = -USEC;
			}
			printf("Prompt End\n");
			printf("Runtime: %ld:%ld(sec:usec)\n", SEC, USEC);
			return 0;
		}
		else if(strcmp(input, "\n") == 0) continue; //just '\n'
		else {
			help();
			continue;
		}
	}
	return 0;
}
