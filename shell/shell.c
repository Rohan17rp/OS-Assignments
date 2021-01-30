#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 5 
#define SIZE 128


int main(){
	char ch;
	char *arg[MAX_ARGS];
	char cmd[SIZE];
	char *token;
	int pid;
	while(1){
		int arg_count = 0, count = 0, i;
		printf("Potato Prompt-->");

		while(1){
			ch = getchar();
			if(ch != '\n'){
				cmd[count] = ch;
				count++;
			}
			else{
				cmd[count] = '\0';
				break;
			}
		}
		
		i = 0;
		token = strtok(cmd, " ");
		while(token != NULL){
			arg[i] = token;
			i++;
			token = strtok(NULL, " ");
		}
		arg[i] = NULL;

		// Printing arguments for debugging
		for(i = 0; i < arg_count; i++){
			printf("%s\n", arg[i]);
		}
		
		// Make copy of process
		pid = fork();
		
		// Child process
		if(pid == 0){
			if(execvp(arg[0], arg) == -1){
				exit(0);
			}
		}
		// Parent process
		else{
			wait(0);
		}
	}
}
