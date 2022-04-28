/*
 * cshell.c
 * 
 * This program implements a shell.
 *
 * Name: Fitz Laddaran
 * Date: February 20, 2022
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
	char *name;
	char *time;
	int retVal;
} Command;

typedef struct {
	char *name;
	char *value;
} EnvVar;

int colour = 0;							// tracker for current colour
int first_EV = 0;							// tracker for first environment variable
int size_EV = 0;							// # of elements in arr_EV[]
int size_Command = 0;							// # of elements in arr_Command[]
Command **arr_Command;							// array: i'th element is Command(i)
EnvVar **arr_EV;							// array: i'th element is EV(i)

void colour_check();							// keeps track of current colour output
void read_input();							// reads user input 
void read_file(FILE *f);						// reads file
void udf_exit();							// built-in function: exit()
int udf_print(char **tokens, int size);				// built-in function: print()
int udf_print_EV(char **tokens);					// built-in function: print() EVs
int udf_theme(char **tokens);						// built-in function: theme()
int udf_log();								// built-in function: log()
int non_built_in_commands(char *input);				// non-built-in functionality
void new_EV(char *input);						// creates a new EV
void add_Command(char *name, int retVal);				// adds Command into arr_Command[]

int main(int argc, char *argv[]) {

	arr_Command = (Command**) malloc(sizeof(Command*) * 512);	
	arr_EV = (EnvVar**) malloc(sizeof(EnvVar*) * 512);		
	
	// interactive mode
	if (argc == 1) {
		while (1) {
			read_input();
		}
	}
	
	// script mode
	else {
		FILE *f = fopen(argv[1], "r");
		read_file(f);
		fclose(f);
	}
	
	return 0;
}

// checks current colour 
void colour_check() {

	if (colour == 1) printf("\x1b[31m");
	else if (colour == 2) printf("\x1b[32m");
	else if (colour == 3) printf("\x1b[34m");

}

// read user input
// reference: https://stackoverflow.com/questions/314401/how-to-read-a-line-from-the-console-in-c
void read_input() {
	
	int size = 0, retVal = 0; 				// number of words
	char first_char;					// first char entered
	char EV_check;						// checks if printing a EV
	char *command;						// command (first arg) entered
	char *second_arg;					// second arg
	char *line = (char*) malloc(sizeof(char) * 512); 	// buffer for user input
	char *copy_line = (char*) malloc(sizeof(char) * 512); // used for strtok()
	char **tokens = (char**) malloc(sizeof(char*) * 512); // number of tokens (words) allowed: 512
	
	colour_check();					
	printf("cshell$ \x1b[0m");
	fgets(line, 512, stdin);
	strcpy(copy_line, line);
	
	// parse user input into tokens
	for (char *token = strtok(copy_line, " "); token != NULL; token = strtok(NULL, " ")) {
		token[strcspn(token, "\n")] = 0;
		tokens[size++] = token;
	}
	
	command = tokens[0];					// first word 
	first_char = command[0];				// first char 
	
	if (size > 1) second_arg = tokens[1];			// second word 
	else second_arg = "0";					// temp value
	
	if (size > 1) EV_check = second_arg[0];		// first char from second arg
	else EV_check = '0';					// temp value
	
	colour_check();
	
	// built-in command: exit
	if (strcmp(command, "exit") == 0) {
		udf_exit();
	}
	
	// built-in command: theme
	else if (strcmp(command, "theme") == 0) {
		retVal = udf_theme(tokens);
		add_Command(command, retVal);
	}
	
	// built-in command: log
	else if (strcmp(command, "log") == 0) {
		retVal = udf_log();
		add_Command(command, retVal);
	}
	
	// built-in command: print
	else if (strcmp(command, "print") == 0) {
		if (EV_check == '$') retVal = udf_print_EV(tokens);
		else retVal = udf_print(tokens, size);
		add_Command(command, retVal);
	}
	
	// environmental variables
	else if (first_char == '$') {
		
		// if no spaces are found (correct syntax)...
		if (size == 1) {
			new_EV(copy_line);
			retVal = 0;
			//printf("copy_line: %s\n", copy_line);
		}
		
		// if spaces are found (wrong syntax)...
		else if (size > 1) {
			printf("Variable value expected (cannot have spaces).\n");
			retVal = 1;
		}
		
		add_Command(copy_line, retVal);
			
	}
	
	// non-built-in commands
	else {
		retVal = non_built_in_commands(line);
		add_Command(command, retVal);
	}
	
	return;
}

// read inputted file (argv[1])
void read_file(FILE *f) {
	
	char *line = (char*) malloc(sizeof(char) * 512);
	
	while (fgets(line, 512, f)) {
	
		char *copy_line = (char*) malloc(sizeof(char) * 512); // used for strtok()
		strcpy(copy_line, line);
		int size = 0, retVal = 0; 				// number of words
		char first_char;					// first char entered
		char EV_check;						// checks if printing a EV
		char *command;						// command (first arg) entered
		char *second_arg;					// second arg
		char **tokens = (char**) malloc(sizeof(char*) * 512); // number of tokens (words) allowed: 512
		
		colour_check();				
		
		// parse user input into tokens
		for (char *token = strtok(copy_line, " "); token != NULL; token = strtok(NULL, " ")) {
			token[strcspn(token, "\n")] = 0;
			tokens[size++] = token;
		}
		
		command = tokens[0];					// first word 
		first_char = command[0];				// first char 
		
		if (size > 1) second_arg = tokens[1];			// second word 
		else second_arg = "0";					// temp value
		
		if (size > 1) EV_check = second_arg[0];		// first char from second arg
		else EV_check = '0';					// temp value
		
		colour_check();
		
		// built-in command: exit
		if (strcmp(command, "exit") == 0) {
			udf_exit();
		}
		
		// built-in command: theme
		else if (strcmp(command, "theme") == 0) {
			retVal = udf_theme(tokens);
			add_Command("theme", retVal);
		}
		
		// built-in command: log
		else if (strcmp(command, "log") == 0) {
			retVal = udf_log();
			add_Command("log", retVal);
		}
		
		// built-in command: print
		else if (strcmp(command, "print") == 0) {
			if (EV_check == '$') retVal = udf_print_EV(tokens);
			else retVal = udf_print(tokens, size);
			add_Command("print", retVal);
		}
		
		// environmental variables
		else if (first_char == '$') {
			
			// if no spaces are found (correct syntax)...
			if (size == 1) {
				new_EV(copy_line);
				retVal = 0;
			}
			
			// if spaces are found (wrong syntax)...
			else if (size > 1) {
				printf("Variable value expected (cannot have spaces).\n");
				retVal = 1;
			}
			
			add_Command(copy_line, retVal);
				
		}
		
		// non-built-in commands
		else {
			retVal = non_built_in_commands(line);
			add_Command(command, retVal);
		}
	}
	
	return;
}

// built-in command: exit
void udf_exit() {
	printf("Bye!\n");
	exit(0);
}

// built-in command: print
int udf_print(char **tokens, int size) {

	for (int i = 1; i < size; i++) {
		printf("%s ", tokens[i]);
	}
	
	printf("\n");
	return 0;
	
}

// built-in command: print EV
int udf_print_EV(char **tokens) {

	for (int i = 0; i < size_EV; i++) {
		if (strcmp(arr_EV[i]->name, tokens[1]) == 0) {
			printf("%s\n", arr_EV[i]->value);
			return 0;
		}
	}
	
	printf("Environmental Variable doesn't exist. Try again.\n");
	return 1;

}

// built-in command: theme
int udf_theme(char **tokens) {
	
	char *color = tokens[1];
	
	if (strcmp(color, "red") == 0) {				// red
		printf("\x1b[31m");
		colour = 1;
	} 
	
	else if (strcmp(color, "green") == 0) {			// green
		printf("\x1b[32m");
		colour = 2;
	}
	
	else if (strcmp(color, "blue") == 0) {			// blue
		printf("\x1b[34m");
		colour = 3;
	}
	
	else {								// not supported colour
		printf("Unsupported theme: %s\n", color);
		return 1;
	}
	
	return 0;
}

// built-in command: log
int udf_log() {

	for (int i = 0; i < size_Command; i++) {
		printf("%s", arr_Command[i]->time);
		printf("%s %d\n", arr_Command[i]->name, arr_Command[i]->retVal);
	}
	
	return 0;
}

// non-built-in commands
// reference: https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection
int non_built_in_commands(char *input) {
	
	int ind = 0;
	char **args = (char**) malloc(sizeof(char*) * 512);	
	
	// parse input string into tokens and print
	for (char *token = strtok(input, " "); token != NULL; token = strtok(NULL, " ")) {
		token[strcspn(token, "\n")] = 0;
		args[ind++] = token;
	}
	
	int pipe_fds[2];					// data structure for pipe()
	
	// if pipe fails...
	if (pipe(pipe_fds) < 0) {
		printf("pipe() failed in non_built_in_commands()! Try again.\n");
		exit(1);
	}
	
	char buffer[1];					// buffer for printing stdout
	
	// if pipe doesn't fail...
	pid_t pid = fork();					// fork() to create child process...
	
	// for child process...
	if (pid == 0) {
		
		close(pipe_fds[0]);				// close read-end: child doesn't need anything from pipe
		int desc = dup2(pipe_fds[1], 1);		// 1 == stdout
		
		// if dup2 fails...
		if (desc < 0) {
			printf("dup2() failed in non_built_in_commands()! Try again.\n");
			exit(1);
		}
		
		close(pipe_fds[1]);				// close write-end: child writing process is finished
		execvp(args[0], args);				// execute linux commands
		
		// child process doesn't get to this unless execvp() "fails", that is: an invalid command is entered
		printf("Missing keyword or command, or permission problem! Try again.\n");
		exit(1);
		
	}
	
	// for parent process...
	else {

		wait(NULL);					// waiting for child to finish its process first
		colour_check();				// replace output colour with current colour
		close(pipe_fds[1]);				// close write-end: parent doesn't need to pass anything to child
		
		// reading what is in stdout file descriptor one character at a time
		while(read(pipe_fds[0], buffer, 1) == 1) {
			printf("%s", buffer);
		}
		
		close(pipe_fds[0]);				// close read-end: parent process is finished

	}
	
	return 0;
}

// creates new EV
void new_EV(char *input) {
	
	size_t ind = 0;
	char **tokens = (char**) malloc(sizeof(char*) * 2);
	
	// parse user input into tokens 
	for (char *token = strtok(input, "="); token != NULL; token = strtok(NULL, "=")) {
		token[strcspn(token, "\n")] = 0;
		tokens[ind++] = token;
	}
	
	for (int i = 0; i < size_EV; i++) {
		if (strcmp(arr_EV[i]->name, tokens[0]) == 0) {
			arr_EV[i]->value = tokens[1];
			return;
		}
	}
	
	EnvVar *EV = (EnvVar*) malloc(sizeof(EnvVar));
	EV->name = tokens[0];
	EV->value = tokens[1];
	arr_EV[size_EV++] = EV;
	return;
	
}

// adds new Command
// reference: https://stackoverflow.com/questions/5141960/get-the-current-time-in-c
void add_Command(char *name, int retVal) {
	
	time_t curr_time;
	time(&curr_time);
	struct tm *info = localtime(&curr_time);
	
	Command *new_Command = (Command*) malloc(sizeof(Command));
	new_Command->name = name;
	new_Command->time = asctime(info);
	new_Command->retVal = retVal;
	
	arr_Command[size_Command++] = new_Command;
	
}

