/*
 * my_current_processes.c
 * 
 * This program displays all running processes on my system, along with their respective process ID.
 *
 * Name: Fitz Laddaran
 * Date: January 27, 2022
 */
 
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>

// function: checks if directory name is numeric
// input: a directory's name
// return value: 1 if numeric, 0 otherwise
int check_pid_name(struct dirent *ptr) {
	
	for (int i = 0; ptr->d_name[i]; i++) {

		if(!isdigit(ptr->d_name[i])) {
			return 0;
		}
	
	}
	
	return 1;
	
}


// function: prints PID number and corresponding process name 
// input: a directory's name
// return value: none
void print_proc(char *d_name) {
	
	FILE *fp;
	char path[512];
	char proc_name[512];
	
	snprintf(path, 512, "/proc/%s/cmdline", d_name);
	fp = fopen(path, "r");
	if (fp == NULL) return;
	
	fscanf(fp, "%s", proc_name);
	printf("%s %s\n", d_name, proc_name);
	
	fclose(fp);
	return;
	
}


int main() {

	struct dirent *ptr;
	const char *path = "/proc";
	
	DIR *directory = opendir(path);
	if (directory == NULL) return 0;                      // check if directory open fails
	
	FILE *fp = fopen("output1.txt", "w");
	if (fp == NULL) return 0;                             // check if .txt file open files
	
	// for every sub-directory...
	while ((ptr = readdir(directory))) {              
		
		// if sub-directory name is numeric...
		if(check_pid_name(ptr)) {
			print_proc(ptr->d_name);              // print PID and name
			fputs(ptr->d_name, fp);               // write PID into text file
			fputs("\n", fp);
		}
		
	}	
	
	fclose(fp);
	closedir(directory);
	return 0;

}
