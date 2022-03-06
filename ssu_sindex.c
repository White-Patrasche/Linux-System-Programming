#include <stdio.h>
#include <string.h>
void find();
void exit();
void help();

int main() {
	char input[100];
	memset(input, 0, sizeof(input));
	printf("20193010>");
	scanf("%s",input);
	if(strcmp(input, "find") == 0) { //input == find

	}
	else if(strcmp(input, "exit") == 0) { //input == exit
		
	}

	return 0;
}
