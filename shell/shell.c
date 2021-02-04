#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 10 
#define SIZE 128

void get_command(char*);
int tokenize_command(char**, char*, char*);
void print_args(char**, int);

int main(){
	char *arg[MAX_ARGS];
	char *argpipe[MAX_ARGS];
	char cmd[SIZE];
	int opid, npid, pfd[2];
	while(1){
		int arg_count = 0, i = 0, arg_pipe_count = 0;
		printf("potato-prompt-->");
	
		// Get user input and tokenize	
		get_command(cmd);	
		arg_pipe_count = tokenize_command(argpipe, cmd, "|");

		if (arg_pipe_count == 1){
			arg_count = tokenize_command(arg, argpipe[i], " "); 	
			opid = fork();
			if(opid == 0){
				if(execvp(arg[0], arg) == -1){
					exit(0);
				}
			}
			else{
				wait(NULL);
			}
		}
		else if (arg_pipe_count == 2){	
			pipe(pfd);
			opid = fork();
			if(opid == 0){
				close(1);
				dup(pfd[1]);
				arg_count = tokenize_command(arg, argpipe[0], " "); 
				execvp(arg[0], arg);
			}
			else{
				npid = fork();
				if(npid == 0){
					close(0);
					dup(pfd[0]);
					arg_count = tokenize_command(arg, argpipe[1], " ");
					execvp(arg[0], arg);
				}
				else{
					wait(NULL);
				}
			}
		}
		else {
			printf("Too many pipes, Can't be processed\n");
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
 * Breaks string by delimiter and saves strings in new array
 * Returns number of strings formed
 * */
int tokenize_command(char** argument, char* command, char* delimiter){
	char *str, *token;
	char *saveptr;
	int i;
	for (i = 0, str = command; ; i++, str = NULL) {
		token = strtok_r(str, delimiter, &saveptr);
		if (token == NULL){
			break;
		}
		argument[i] = token;
		//printf("%d: %s\n", i, token);
	}
	argument[i] = NULL;
	return i;
}

/*
 * Prints each string in array on new line
 * */
void print_args(char** arg, int arg_count){
	int i =0;
	for(i = 0; i < arg_count; i++){
		printf("%s\n", arg[i]);
	}
}
