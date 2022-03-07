#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#define INPUT_SIZE 0xffff
void find();
void exit();
void help();

void Print_time(time_t TIME) {
	struct tm *t;
	//t = localtime(TIME);
	//printf("%d/%d/\n",1900+now.tm_year+1900, now.tm_mon+1);
}

void find_fuc(char* INPUT) {
	struct stat* buf;
	printf("Index Size Mode      Blocks Links UID  GID  Access    Change         Modify         Path\n");
	stat("/a", buf);
	printf("%ld",buf->st_atime);
	//Print_time(buf->st_atime);
	printf("UID: %d, GID : %d\n",buf->st_mode, buf->st_uid);
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
			printf("Ptompt End\n");
			printf("runtime: %ld:%ld(sec:usec)\n", SEC, USEC);
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
