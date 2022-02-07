/************************************
 * author: Daniel Trang
 * class: CS333
 *
 * This file contains the implmentation
 * of a simple and silly linux shell in
 * C-language. The functionalities are
 * very limited. The purpose of this
 * exercise is to demonstrate foreground
 * vs. background processes as well as
 * handling signals on child process exit.
 * ***********************************/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define clear() printf("\033[H\033[J")
#define MAX_SIZE 1024
#define MAX_SIZE_ARG 100

//function forward declarations...
void shell_init();
void promptUser();
void collectInput(char *input);
int parseInput(char input[], char *command[]);
void executeCommand(char *command[], int index_val);
void handle_sigchld(int sig);
void register_sigchld();

int main(){
	//initialize variables...
	char user_input[MAX_SIZE];
	char * argv[MAX_SIZE_ARG];
	int index_val = 0;

	//initialize shell...
	shell_init();

	//register a SIGCHLD
	register_sigchld();

	while(1){
		//prompt user for input
		promptUser();		

		//take user input
		collectInput(user_input);

		//Ignore empty commands
		if(!strcmp(user_input, "")) continue;

		//parse and process valid input/args
		index_val = parseInput(user_input, argv);	

		//execute the command 
		executeCommand(argv, index_val);	
	}
	return 0;
}


//function implementations...

void shell_init(){
	clear();
	printf("\n\n\n***************************************");
	printf("\n*%38s", "*");
	printf("\n*%38s", "*");
	printf("\n*%13sSILLY SHELL%14s", "", "*");
	printf("\n*%38s", "*");
	printf("\n*%38s", "*");
	printf("\n***************************************");
	printf("\nDisclaimer: This is a mock linux shell. ");
	printf("\nUse for learning purposes only.");
	printf(" The \nfunctionalities are limited and may not ");
	printf("\nbe accurate to actual shell functionalities.");

	char* username = getenv("USER");
	printf("\n\n\nUSER: @%s", username);
	printf("\n");
	sleep(1);
}

//UI prompt for user
void promptUser(){
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("%s", cwd);
	printf("$ sillyshell> ");
}

//Actual user input collection stored in array
void collectInput(char input[]){
	//take in user input and store into input array
	fgets(input, MAX_SIZE, stdin);
	//remove newline and replace with null character
	if((strlen(input) > 0) && (input[strlen(input) - 1] == '\n')){
		input[strlen(input) - 1] = '\0';
	}
}

//parsing user input as string and storing into command[] array
//check for ampersand (&) character to run command in background
int parseInput(char input[], char * command[]){		
	int i = 0;

	//tokenize user input, then store in command[i]
	char * token = strtok(input, " ");
	while(token != NULL){
		command[i] = token;
		token = strtok(NULL, " ");
		i++;
	}

	//check for ampersand(&)
	if(!strcmp(command[i-1], "&")){
		command[i-1] = NULL;
		command[i] = "&";
	}	
	else{
		command[i] = NULL;
	}

	//return value of integer i through index_val
	return i;
}

//fork child process and execute command
void executeCommand(char *command[], int index_val){
	//variable to hold status
	int status=0;
	//fork process
	pid_t pid = fork();

	//if "exit" command
	if(strcmp(command[0], "exit") == 0){
		//catch any child processes before exit
		while(waitpid(-1, &status, 0) > 0){}
		if(pid > 0){
			printf("Exiting sillyshell. Goodbye.\n");
		}
		else{
			printf("Waiting for any child processes...\n");
		}
		exit(0);
	}

	if(pid == 0){
		//this is child process
		if(execvp(command[0], command) == -1){
			perror("Error executing command.");
		}
		exit(0);
	}
	else if(pid < 0){
		perror("Could not fork...");
	}
	else{
		if(command[index_val] == NULL){
			//process running in foreground
			if(waitpid(pid, &status, 0) < 0){
				printf("wait_foreground: waitpid error");
			}
		}
		else{ //tell user that background process is added
			printf("%d %s\n", pid, command[0]);
		}
	}
}

//register the SIGCHLD struct and calling sigaction
void register_sigchld(){
	struct sigaction sa;
	sa.sa_handler = &handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if (sigaction(SIGCHLD, &sa, 0) == -1){
		perror(0);
		exit(1);
	}
}

//signal handler for SIGCHLD
void handle_sigchld(int sig){
	int status;
	waitpid((pid_t)(-1), &status, WNOHANG);
}
