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
list_manage* makeList();
void pop(list_manage*);
char* front(list_manage*);
void push(list_manage*, char*, int, int, int);
void clear(list_manage*);
void error();
void help();
void inputClear(char*, int);
void error(int);
list_manage* find_func(char*, char*);
list_manage* makeList();
list_manage* find_dir();
void endList(list_manage*);
long sizeOfDir(char*);
void flush();
//list_manage* find_file();
int printData(list_manage*);
//void indexSearch(list_manage*);

//functions
list_manage* makeList() {
	list_manage* manager = malloc(sizeof(list_manage));
	file_data* start = malloc(sizeof(file_data));
	file_data* end = malloc(sizeof(file_data));
	start->prev = end->next = NULL;
	start->next = end;
	end->prev = start;

	inputClear(start->path, MAX);
	inputClear(end->path, MAX);
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

void flush() {
	while(getchar() != '\n');
}

list_manage* find_dir(list_manage* manager, char* fname) {
	char dpath[MAX]; //start scan
	long dsize = manager->begin->next->size;
	int level = manager->begin->next->level;
	strcpy(dpath, manager->begin->next->path);
	list_manage* onlyDir = makeList(); //to return list
	list_manage* scan = makeList();
	push(onlyDir, dpath, level, 2, dsize);
	//to scan from dpath, delete elements before first /
	//if i == 0, it's root dir
	printf("%s\n",dpath);
	for(int i=strlen(manager->begin->next->path)-1; i>0; i--) { 
		if(dpath[i] != '/') dpath[i] = '\0';
		else {
			dpath[i] = '\0';
			break;
		}
	}

	push(scan, dpath, level, 2, dsize);
	struct stat st;
	struct dirent **namelist;
	long size;
	int cnt, idx;
	while(1) {
		if(scan->size == 0) break;
		strcpy(dpath, front(scan));
		level = scan->begin->next->level + 1;
		pop(scan);
		for(idx=0; idx<cnt; idx++) {
			size = 0;
			if(!strcmp(namelist[idx]->d_name, ".") ||
					!strcmp(namelist[idx]->d_name, "..")) continue;
			if(!strcmp(namelist[idx]->d_name, "proc") ||
					!strcmp(namelist[idx]->d_name, "run")) continue;
			char newPath[MAX];
			if(!strcmp(dpath, "/")) { //find from root dir
				strcpy(newPath, "/");
				strcat(newPath, namelist[idx]->d_name);
			}
			else {
				strcpy(newPath, dpath);
				strcat(newPath, "/");
				strcat(newPath, namelist[idx]->d_name);
			}
			stat(newPath, &st);
			if((st.st_mode & S_IFMT) == S_IFDIR) {
				if(!strcmp(namelist[idx]->d_name, fname)) {
					size = sizeOfDir(newPath);
					if(dsize == size) { //find it!
						push(onlyDir, newPath, level, 2, size);
						printf("%s add!\n",newPath);
					}
				}
				push(scan, newPath, level, 2, size);
			}
		}
	}
	endList(scan);
	return onlyDir;
}
/*
list_manage* find_file(list_manage* manager) {}
int printData(list_manage* manager) {
}
void indexSearch(list_manage* manager) {} 
*/

int printData(list_manage* manager) {
	file_data* cur = manager->begin->next;
	struct stat st;
	for(int i=0; i<manager->size; i++) {
		stat(cur->path,&st);
		printf("path : %s, fsize : %ld, mode : %o\n",cur->path, cur->size,
				st.st_mode);
		cur = cur->next;
	}
	return 0;
}

long sizeOfDir(char* path) {
	long size = 0;
	struct dirent **namelist;
	struct stat st;
	int cnt = scandir(path, &namelist, NULL, alphasort);
	if(cnt < 0) return 0;
	for(int i=0; i<cnt; i++) {
		char tempPath[MAX];
		strcpy(tempPath, path);
		strcat(tempPath, "/");
		strcat(tempPath, namelist[i]->d_name);
		stat(tempPath, &st);
		if((st.st_mode & S_IFREG) == S_IFREG) {
			size += st.st_size;
		}
	}
	free(namelist);
	return size;
}

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
			if(!strcmp(namelist[idx]->d_name, "proc") ||
					!strcmp(namelist[idx]->d_name, "run")) continue;
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
			stat(newPath, &st);
			if(!strcmp(namelist[idx]->d_name, fname)) {
				flag = 1;
				strcpy(ans, newPath);
				if((st.st_mode & S_IFMT) == S_IFDIR) isDir = 2; //DIR
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

void inputClear(char* INPUT, int size) {
	for(int i=0; i<size; i++) INPUT[i] = '\0';
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
		inputClear(input, MAX);
		printf("20193010> ");
		fgets(input,MAX,stdin); //input includes '\n'
		input[strlen(input)-1] = '\0';
		INDEX = 0;
		char *ptr, *fname, *path, *val;
		if(input[0] == '\0') continue;
		ptr = strtok(input, " ");
		val = ptr;
		if(!strcmp(val, "find")) {
			ptr = strtok(NULL, " ");
			fname = ptr;
			ptr = strtok(NULL, " ");
			path = ptr;
			printf("%s %s",ptr,path);
			if(fname == NULL) {
				error(0); //file name missed
				continue;
			}
			else if(path == NULL) {
				error(1); //path missed
				continue;
			}
			
			list_manage* mode = find_func(fname, path);
			list_manage* actual_list;
			printf("%s\n",mode->begin->next->path);
			switch(mode->begin->next->isDir) {
				case 2: //when find dir
					printf("FIND_DIR FUNC\n");
					actual_list = find_dir(mode, fname);
					break;
				case 1: //when find file
					//actual_list = find_file(mode);
					break;
				case 3: //can't find file or dir
					printf("can't find dir or file!\n");
					endList(mode);
					continue;
			}

			if(printData(actual_list)) { //find extra data
				//indexSearch(actual_list);
			}
			endList(actual_list);
			endList(mode);
		}
	
		else if(strcmp(ptr, "exit") == 0) { //input == exit
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
		else {
			help();
			continue;
		}
	}
	exit(0);
}
