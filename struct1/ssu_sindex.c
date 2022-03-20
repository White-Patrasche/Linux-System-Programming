#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#define MAX 4096

//global variables
int INDEX = 0; //searched index size
char global_path[MAX] = "";

//struct declaration
typedef struct File_data {
	char path[MAX];
	int level; //from original path, how much is it level
	struct File_data* prev;
	struct File_data* next;
	int isDir; // 1:dir, 2:file, 3:begin or end
	long size;
}file_data;

typedef struct List_manage {
	struct File_data* begin;
	struct File_data* end;
	int size;
}list_manage;

//function declare
void help();
void inputClear(char*);
void error(int);
list_manage* find_func(char*, char*);
list_manage* makeList();
//list_manage* find_dir();
//list_manage* find_file();
//int printData(list_manage*);
//void indexSearch(list_manage*);

//functions
list_manage* makeList() {
	list_manage* manager = malloc(sizeof(list_manage));
	file_data* start = malloc(sizeof(file_data));
	file_data* end = malloc(sizeof(file_data));
	start->prev = end->next = NULL;
	start->next = end;
	end->prev = start;

	inputClear(start->path);
	inputClear(end->path);
	start->level = 0;
	end->level = MAX;
	start->isDir = end->isDir = 3;
	start->size = end->size = 0;

	manager->begin = start;
	manager->end = end;
	manager->size = 0;
	return manager;
}

void pop(list_manage* manager) {
	file_data* cur = manager->begin->next;
	manager->begin->next = cur->next;
	cur->next->prev = manager->begin;
	free(cur);
	manager->size--;
}

char* front(list_manage* manager) {
	file_data* cur = manager->begin->next;
	return cur->path;
}

void push(list_manage* manage, char* path, int level, int isDir, int size) {
	file_data* cur = manage->begin;
	while(1) {
		if(cur->level > level) break;
		else cur = cur->next;
	}
	file_data* newNode = malloc(sizeof(file_data));
	newNode->prev = cur->prev;
	newNode->next = cur;
	newNode->level = level;
	strcpy(newNode->path, path);
	newNode->isDir = isDir;
	cur->prev->next = newNode;
	cur->prev = newNode;
	newNode->size = size;
	manage->size++;
}

void clear(list_manage* manager) {
	for(int i=0; i<manager->size; i++) {
		file_data* cur = manager->begin->next;
		manager->begin->next = cur->next;
		free(cur);
	}
	manager->begin->next = manager->end;
	manager->end->prev = manager->begin;
	manager->size = 0;
}

void endList(list_manage* manager) {
	for(int i=0; i<manager->size; i++) {
		file_data* cur = manager->begin->next;
		manager->begin->next = cur->next;
		free(cur);
	}
	free(manager->begin);
	free(manager->end);
	manager->size = 0;
	manager->begin = NULL;
	manager->end = NULL;
	free(manager);
}

void error(int errorNum) {
	switch(errorNum) {
		case 1 :
			printf("INPUT data \"File Name\" is not casted\n");
			break;
		case 2 :
			printf("INPUT data \"Path\" is not casted\n");
			break;
	}
}

/*
list_manage* find_dir(list_manage* manager) {}
list_manage* find_file(list_manage* manager) {}
int printData(list_manage* manager) {}
void indexSearch(list_manage* manager) {} 
*/


list_manage* find_func(char* fname, char* path) {
	list_manage* manager = makeList();
	//manager is linked list that queue that sorted by level of directory
	//from original path
	char RealPath[MAX]; //path -> realpath
	if(realpath(path, RealPath) == NULL) {
		return manager;
	}
	struct dirent **namelist;
	int cnt, idx;
	int level = 0, isDir = 0, flag = 0;
	long size = 0;
	char ans[MAX];
	struct stat st;
	push(manager, RealPath, level, 2, 0);
	while(1) {
		flag = 0;
		level = manager->begin->next->level;
		if(manager->size == 0) break;
		strcpy(RealPath, front(manager));
		if((cnt = scandir(RealPath, &namelist, NULL, alphasort)) == -1) {
			fprintf(stderr, "%s Directory scan error\n", RealPath);
			pop(manager);
			continue;
		}
		for(idx=0; idx<cnt; idx++) {
			if(!strcmp(namelist[idx]->d_name, ".")) continue;
			if(!strcmp(namelist[idx]->d_name, "..")) continue;
			char newPath[MAX];
			strcpy(newPath, "");
			if(!strcmp(RealPath, "/")) { //it start finding from root dir
				strcat(newPath, "/");
				strcat(newPath, namelist[idx]->d_name);
			}
			else { //it's not root directory
				strcat(newPath, RealPath);
				strcat(newPath, "/");
				strcat(newPath, namelist[idx]->d_name);
			}
			printf("%s\n",newPath);
			if(!strcmp(newPath, "/proc") || !strcmp(newPath, "/run") ||
					!strcmp(newPath, "/usr"))
				continue;
			stat(newPath, &st);
			if(!strcmp(namelist[idx]->d_name, fname)) {
				flag = 1;
				strcpy(ans, newPath);
				if((st.st_mode & S_IFMT) == S_IFDIR) isDir = 2;
				else isDir = 1;
				break;
			}
			if((st.st_mode & S_IFMT) == S_IFDIR) {
				push(manager, newPath, level+1, 2, 0);
			}
			free(namelist[idx]);
		}
		if(flag) break; //not zero
		pop(manager);
		free(namelist);
	}
	clear(manager);
	if(flag) { //find it?
		if(isDir == 2) { //it's dir
			int cnt = scandir(ans, &namelist, NULL, alphasort);
			for(int i=0; i<cnt; i++) {
				char tempPath[MAX];
				strcpy(tempPath, "");
				strcat(tempPath, ans);
				strcat(tempPath, "/");
				strcat(tempPath, namelist[i]->d_name);
				printf("%s is newPath\n", tempPath);
				stat(tempPath, &st);
				if((st.st_mode & S_IFREG) == S_IFREG) {
					size += st.st_size;
				}
			}
			free(namelist);
		}
		else if(isDir == 1) { //it's file
			stat(ans, &st);
			size = st.st_size;
		}
		push(manager, ans, level, isDir, size);
	}
	return manager;	
}

//	printf("Index Size Mode      Blocks Links UID  GID  Access    Change         Modify         Path\n");

void inputClear(char* INPUT) {
	for(int i=0; i<MAX; i++) INPUT[i] = 0;
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

int main(void) {
	char input[MAX];

	struct timeval start_time; //time check
	gettimeofday(&start_time, NULL);

	while(1) {
		memset(input, 0, sizeof(input)); //input memory reset
		printf("20193010> ");
		fgets(input,MAX,stdin); //input includes '\n'
		inputClear(input);
		INDEX = 0;
		char *ptr, *ptr1, *ptr2;
		ptr = strtok(input, " ");
		if(strcmp(ptr, "find") == 0) {
			ptr = strtok(NULL, " ");
			ptr1 = ptr;
			ptr = strtok(NULL, " \n");
			ptr2 = ptr;
			if(strcmp(ptr1, "\n") == 0) {
				error(0);
				continue;
			}
			else if(strcmp(ptr2, "\n") == 0) {
				error(1);
				continue;
			}
			list_manage* mode = find_func(ptr1, ptr2);
			//list_manage* actual_list
			switch(mode->begin->next->isDir) {
				case 1:
					//actual_list = find_dir(mode);
					break;
				case 2:
					//actual_list = find_file(mode);
					break;
				case 3:
					printf("can't find dir or file!\n");
					endList(mode);
					continue;
			}
			/*
			if(printData(actual_list)) { //find extra data
				indexSearch(actual_list);
			}
			endList(actual_list);
			*/
			endList(mode);
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
	exit(0);
}
