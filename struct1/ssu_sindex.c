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
void exit();
void help();
int INDEX = 0;
void find_dir(char* FILENAME, char* PATH) {
	char RealPath[INPUT_SIZE];
	printf("%s\n",PATH);
	if(realpath(PATH, RealPath) == NULL) {
		printf("RealPath translating error\n");
		return;
	}
	int cnt;
	struct dirent ** DIR;
	printf("Index Size Mode      Blocks Links UID  GID  Access    Change         Modify         Path\n");
	cnt = scandir(RealPath, &DIR, NULL, alphasort);
	for(int i=0; i<cnt; i++) {
		printf("%s\n",DIR[i]->d_name);
	}
	/*
	while(1) { //translate to realpath	
		realpath(
	}
	*/
}

void find_fuc(char* INPUT) {
	struct stat buf;
	printf("Index Size Mode      Blocks Links UID  GID  Access    Change         Modify         Path\n");
	stat("b", &buf);
	int INDEX = 0;
	printf("%-6d",INDEX);
	printf("%-5lld",(long long)buf.st_size); //file size
	printf("%lo",(unsigned long)buf.st_mode);//Mode
	printf("%-7ld",(long)buf.st_blksize); //blocks
	printf("%-6ld",(long)buf.st_nlink); //links
	printf("%-5ld",(long)buf.st_uid);
	printf("%-5ld",(long)buf.st_gid);
	//access time
	printf("\n");
	int count;
	struct dirent **DIR;
	char NEWPATH[1024];
	if((count = scandir(".", &DIR, NULL, alphasort)) == -1) return;

	//for(int i=0; i<count; i++) printf("%s\n",DIR[i]->d_name);

	for(int i=0; i<count; i++) {
		realpath(DIR[i]->d_name, NEWPATH);
		printf("%s\n",NEWPATH);
	}


	for(int i=0; i<count; i++) free(DIR[i]);
	free(DIR);
	return;
}

void inputClear(char* INPUT) {
	for(int i=0; i<INPUT_SIZE; i++) INPUT = 0;
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

	struct timeval start_time; //time check
	gettimeofday(&start_time, NULL);

	while(1) {
		memset(input, 0, sizeof(input)); //input memory reset
		printf("20193010> ");
		fgets(input,INPUT_SIZE,stdin); //input includes '\n'i
		char *ptr, *ptr1, *ptr2;
		ptr = strtok(input, " ");
		if(strcmp(ptr, "find") == 0) {
			ptr = strtok(NULL, " \n");
			ptr1 = ptr;
			ptr = strtok(NULL, " \n");
			ptr2 = ptr;
			find_dir(ptr1, ptr2);
		}
	
		else if(strcmp(ptr, "exit\n") == 0) { //input == exit
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
		else if(strcmp(ptr, "\n") == 0) continue; //just '\n'
		else {
			help();
			continue;
		}
	}
	return 0;
}
