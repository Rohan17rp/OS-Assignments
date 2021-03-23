#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_ARGS 10 
#define SIZE 128

char* remove_spaces(char *string);

void get_command(char*);
int tokenize_command(char**, char*, char*);
void print_args(char**, int);
void pipeexec(int* pfd,char** arg, char** argpipe, int, int);
void cmdexec(char** arg, char** argpipe, int opid);

void pipeexec2(int* pfd, char** arg, char** argpipe, int opid, int npid, int arg_pipe_count);

int main(){
	char *arg[MAX_ARGS];
	char *argpipe[MAX_ARGS];
	char cmd[SIZE];
	int opid, npid, pfd[2];
	while(1){
		int arg_count = 0, i = 0, arg_pipe_count = 0;
		printf("potato-prompt-->");
	
		get_command(cmd);	
		arg_pipe_count = tokenize_command(argpipe, cmd, "|");

		if (arg_pipe_count == 1){
	//		int input_redirection = 0;
	//		int output_redirection = 0;
	//		int fd;
	//		
	//		input_redirection = tokenize_command(arg, argpipe[0], "<");
	//		//printf("\n%d input redirections\n", input_redirection - 1);
	//		if(input_redirection == 2){
	//			char* file_name = remove_spaces(arg[1]);
	//			fd = open(file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	//			dup2(fd, 0);
	//			close(fd);
	//		}
	//		else if (input_redirection > 2){
	//			printf("Too many input redirections\n");
	//		}
	//		output_redirection = tokenize_command(arg, argpipe[0], ">");
	//		//printf("%d output redirections\n", output_redirection - 1);
	//		if(output_redirection == 2){
	//			char* file_name = remove_spaces(arg[1]);
	//			fd = open(file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	//			dup2(fd, 1);
	//			close(fd);
	//		}
	//		else if (output_redirection > 2){
	//			printf("Too many output redirections\n");
	//		}
	//		close(fd);
			cmdexec(arg, argpipe, opid);
		}
		else if (arg_pipe_count == 2){	
			pipeexec(pfd, arg, argpipe, opid, npid);
		}
		else {
			pipeexec2(pfd, arg, argpipe, opid, npid, arg_pipe_count);
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

void cmdexec(char** arg, char** argpipe, int opid){
	int input_redirection = 0;
	int output_redirection = 0;
	int fd;
	opid = fork();
	if(opid == 0){	
		output_redirection = tokenize_command(arg, argpipe[0], ">");
		//printf("%d output redirections\n", output_redirection - 1);
		if(output_redirection == 2){
			char* file_name = remove_spaces(arg[1]);
			fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
			dup2(fd, 1);
			close(fd);
		}
		else if (output_redirection > 2){
			printf("Too many output redirections\n");
		}
		
		input_redirection = tokenize_command(arg, argpipe[0], "<");
		//printf("\n%d input redirections\n", input_redirection - 1);
		if(input_redirection == 2){
			char* file_name = remove_spaces(arg[1]);
			fd = open(file_name, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
			dup2(fd, 0);
			close(fd);
		}
		else if (input_redirection > 2){
			printf("Too many input redirections\n");
		}

		tokenize_command(arg, argpipe[0], " "); 	

		if(execvp(arg[0], arg) == -1){
			printf("Exec Failed");
			exit(0);
		}
	}
	else{
		wait(NULL);
	}
}

void pipeexec(int* pfd, char** arg, char** argpipe, int opid, int npid){
	pipe(pfd);
	opid = fork();
	if(opid == 0){
		close(1);
		dup(pfd[1]);
		tokenize_command(arg, argpipe[0], " "); 
		execvp(arg[0], arg);
	}
	else{
		npid = fork();
		if(npid == 0){
			close(0);
			dup(pfd[0]);
			tokenize_command(arg, argpipe[1], " ");
			execvp(arg[0], arg);
		}
		else{
			wait(0);
		}
	}
}

void pipeexec2(int* pfd, char** arg, char** argpipe, int opid, int npid, int arg_pipe_count){
	pipe(pfd);
//	opid = fork();
	int i = 0;	
	for(i = 0; i < arg_pipe_count; i++){
		npid = fork();
		if(npid == 0){
			if(i != 0){
				close(0);
				dup(pfd[0]);
			}
			if(i != arg_pipe_count - 1){
				close(1);
				dup(pfd[1]);
			}
			tokenize_command(arg, argpipe[i], " ");
			execvp(arg[0], arg);
		}
	}
}

char* remove_spaces(char *string){
	int size = strlen(string);
	char* file_name = malloc(sizeof(char) * size);
	int count = 0;
	int i;
	for(i = 0; i < size; i++){
		if(string[i] == '\0'){
			break;
		}
		if(string[i] != ' '){
			file_name[count] = string[i];
			count++;
		}
	}
	if(file_name[count - 1] != '\0'){
		file_name[count] = '\0';
	}
	return file_name;
}

