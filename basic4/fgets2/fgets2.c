#include <stdio.h>
#include <stdlib.h>

#define BUFFER_MAX 1024

int main() {
	char command[BUFFER_MAX];
	char *prompt = "Prompt>>";

	while(1) {
		fputs(prompt, stdout);

		if(fgets(command, sizeof(command), stdin) == NULL)
			break;
		
		system(command);
	}

	fprintf(stdout, "Good bye...\n");
	fflush(stdout);
	exit(0);
}
