#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 5 
#define SIZE 128

void get_command(char*);
int tokenize_command(char**, char*);
void print_args(char** arg, int arg_count);

int main(){
	char *arg[MAX_ARGS];
	char cmd[SIZE];
	int pid;
	while(1){
		int arg_count = 0, i;
		printf("potato-prompt-->");
	
		// Get user input	
		get_command(cmd);
		
		arg_count = tokenize_command(arg, cmd);	

		// Printing arguments for debugging
		// print_args(arg, arg_count);

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

/*
 * Get input from user
 * */
void get_command(char* cmd){
	char ch;
	int count = 0;
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
}

/*
 * Breaks string by spaces and saves strings in new array
 * Returns number of strings formed
 * */
int tokenize_command(char** arg, char* cmd){
	char *token;
	int i = 0;
	token = strtok(cmd, " ");
	while(token != NULL){
		arg[i] = token;
		i++;
		token = strtok(NULL, " ");
	}
	arg[i] = NULL;
	return i;
}

void print_args(char** arg, int arg_count){
	int i =0;
	for(i = 0; i < arg_count; i++){
		printf("%s\n", arg[i]);
	}
}
