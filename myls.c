/*
 * Implementation of linux 'ls' command
 * Author:	Fitz Laddaran
 * Date:      	April 09, 2022
 *
 * ./myls [options] [file path]
 *
 *
 ****************************************************************
 * References:
 *
 * https://stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c
 * https://iq.opengenus.org/ls-command-in-c/
 * https://stackoverflow.com/questions/13542345/how-to-convert-st-mtime-which-get-from-stat-function-to-string-or-char
 * https://stackoverflow.com/questions/8812959/how-to-read-linux-file-permission-programmatically-in-c-c
 ****************************************************************
 */



#include <time.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int flag_i = 0;								// flag: track if -i [option] has been specified
int flag_l = 0;								// flag: track if -l [option] has been specified
int flag_R = 0;								// flag: track if -R [option] has been specified
int flag_output_dir_name = 0;							// flag: track if more than one [file path] has been specified
int flag_file = 0;								// flag: track if at least one [file path] has been specified

void ls(struct dirent **iter, char *dir, int n);				// executes 'ls' command
void ls_file(struct stat s, char *filename);					// executes 'ls' command for files
void set_flags(char *option);							// sets flags
void print_i(struct stat s);							// executes -i [option] print statements
void print_l(struct stat s);							// executes -l [option] print statements
char *check_perms(struct stat s);						// checks permission bits for -l [option] 


int main(int argc, char *argv[]) {

	int i = 1, j = 0, k = 0, n;						
	char **file_list = (char**) malloc(sizeof(char*) * argc);
	char *arg = argv[i];
	char *dir;
	struct dirent **iter;
	struct stat s;
	char buffer[1024];
	
	// iterate through arguments (user input)... 
	while (arg != NULL) {
		
		// if argument is an [option]...
		if (arg[0] == '-') set_flags(arg);
		
		// if argument may be a [file path]...
		else {
			flag_file = 1;
			file_list[j++] = argv[i];
		}
		
		arg = argv[++i];						// next element (user input)
		
	}
	
	if (flag_file == 0) file_list[j++] = ".";				// if no [file path] arg, set to current working directory
	if (j > 1) flag_output_dir_name = 1;					// if >1 [file path] arg, output directory name
	dir = file_list[k];							// set directory to first [file path] in file_list

	// while there are still [file path]s to iterate through...
	while (k < j) {
	
		sprintf(buffer, "%s", file_list[k]);
		
		// if file_list[k] is not a file or directory...
		if (stat(buffer, &s) != 0) {
			printf("ls: cannot access '%s': No such file or directory\n", buffer);
		} 
		
		// if file_list[k] is a file...
		else if (S_ISDIR(s.st_mode) == 0) {
			ls_file(s, file_list[k]);
		}
		
		// if file_list[k] is a directory...
		else {
			
			// scanning directory...
			if ((n = scandir(dir, &iter, NULL, alphasort)) == -1) {
				printf("Error: scandir() failed\n");
				exit(EXIT_FAILURE);
			} 
			
			ls(iter, dir, n);					// execute 'ls' command
			while (n--) free(iter[n]);				// free blocks of memory
							
		}
		
		dir = file_list[++k]; 						// next [file path]...
		printf("\n");	
	}
	
	free(iter);								// free blocks of memory
	free(file_list);							// free blocks of memory
	
}


// function: 'ls' command execution
// paramaters: (array of files, file path, number of files in file path)
void ls(struct dirent **iter, char *dir, int n) {
								
	char **file_list = (char**) malloc(sizeof(char*) * n);		// at most n possible directories to traverse through for -R flag
	
	struct stat s;
	int i = 0 , j = 0, k = 0;
	char buffer[1024];							// buffer for [file path]

	// check if name [file path] needs to be printed first...
	if (flag_output_dir_name == 1 || flag_R == 1) {
		printf("%s:\n", dir);
	}
	
	// for every element in [file path]...
	while (i < n) {
		
		// if file name does not start with '.'
		if (iter[i]->d_name[0] != '.') {
		
			sprintf(buffer, "%s/%s", dir, iter[i]->d_name);

			// if stat() fails...
			if (stat(buffer, &s) != 0) {
				printf("stat() failed\n");
				exit(EXIT_FAILURE);
			}  
			
			// check flags...
			if (flag_i == 1) print_i(s);				
			if (flag_l == 1) print_l(s);
			printf("%s \n", iter[i]->d_name);
			if (flag_R == 1 && S_ISDIR(s.st_mode)) file_list[j++] = strdup(buffer);
			
		}
		
		i++;
		
	}
	
	// if there are directories to traverse through (-R [option] MUST be specified)...
	if (j > 0) {
	
		struct dirent **new_iter;
		char *new_dir = file_list[k];
		int m;
	
		// while there are more directories to traverse through...
		while (k < j) {
		
			if ((m = scandir(new_dir, &new_iter, NULL, alphasort)) == -1) {
				printf("Error: scandir() failed\n");
				exit(EXIT_FAILURE);
			} 
			
			printf("\n");
			ls(new_iter, new_dir, m);				// recursive call for every directory...
			while (m--) free(new_iter[m]);			// free blocks of memory
			new_dir = file_list[++k]; 				// next directory...
			
		}
		
	}
	
	free(file_list);							// free blocks of memory
	
}


// function: 'ls' command execution for files
// parameters: (file information)
void ls_file(struct stat s, char *filename) {

	// check flags...
	if (flag_i == 1) print_i(s);				
	if (flag_l == 1) print_l(s);
	printf("%s\n", filename);
	
}

// function: sets flags -i, -l and -R
// parameters: (user input starting '-')
void set_flags(char *option) {

	int i = 1;
	
	// check what [option(s)] have been specified...
	while (option[i] != '\0') {
		
		// if [option] is unsupported...
		if (option[i] != 'i' && option[i] != 'l' && option[i] != 'R') {
			printf("Error: Unsupported Option\n");
			exit(EXIT_FAILURE);
		}
		
		// if [option] is supported...
		else if (option[i] == 'i') flag_i = 1;			// set flag for -i
		else if (option[i] == 'l') flag_l = 1;			// set flag for -l
		else if (option[i] == 'R') flag_R = 1;			// set flag for -R
		i++;
			
	} 
	
}


// function: prints elements of -i option
// parameters: (file information)
void print_i(struct stat s) {
	printf("%ld ", s.st_ino);
}


// function: prints elements of -l option
// parameters: (file information)
void print_l(struct stat s) {

	struct tm t;
	struct passwd *p;
	struct group *g;
	time_t time;
	char *perms;
	char date[18];								// MMM_DD_YYYY_TT:TT
	
	time = s.st_mtime;
	localtime_r(&time, &t);
	strftime(date, 18, "%b %e %Y %H:%M", &t);

	p = getpwuid(s.st_uid);						// get user ID (name)
	g = getgrgid(s.st_gid);						// get group ID (name)
	perms = check_perms(s);						// obtaining permission bits
	
	printf("%s %ld %s %s %5lu %s ", perms, s.st_nlink, p->pw_name, g->gr_name, s.st_size, date);
	free(perms);								// free blocks of memory
	
}


// function: checks permissions for -l option
// parameters: (file information)
char *check_perms(struct stat s) {

	/*
	 * file/dir-10 permission bits
	 * file/dir-r-w-x-r-w-x-r-w-x
	 */
	 
	char *perms = (char *) malloc(sizeof(char) * 11);
	mode_t perm = s.st_mode;
	
	perms[0] = (perm & S_IFDIR) ? 'd' : '-';				// check: file or directory
	perms[1] = (perm & S_IRUSR) ? 'r' : '-';				// owner: read 
	perms[2] = (perm & S_IWUSR) ? 'w' : '-';				// owner: write
	perms[3] = (perm & S_IXUSR) ? 'x' : '-';				// owner: execute
	perms[4] = (perm & S_IRGRP) ? 'r' : '-';				// group: read
	perms[5] = (perm & S_IWGRP) ? 'w' : '-';				// group: write
	perms[6] = (perm & S_IXGRP) ? 'x' : '-';				// group: execute
	perms[7] = (perm & S_IROTH) ? 'r' : '-';				// other users: read
	perms[8] = (perm & S_IWOTH) ? 'w' : '-';				// other users: write
	perms[9] = (perm & S_IXOTH) ? 'x' : '-';				// other users: execute
	perms[10] = '\0';
	return perms;
	
}






