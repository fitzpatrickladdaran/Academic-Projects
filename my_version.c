/*
 * my_version.c
 * 
 * This program displays the operating system's information.
 *
 * Name: Fitz Laddaran
 * Date: January 27, 2022
 */

#include <stdio.h>

int main() {

	char info[1024];	
	
	FILE *etc = fopen("/etc/os-release", "r");
	if (etc == NULL) return 0;
	
	FILE *output3 = fopen("output3.txt", "w");
	if (output3 == NULL) return 0;
	
	while (fgets(info, sizeof(info), etc)) {
	
		// directing every outputted line into output3.txt
		fputs(info, output3);
		
	}
	
	fclose(etc);
	
	FILE *proc = fopen("/proc/version", "r");
	if (proc == NULL) return 0;
	
	// directing output into output3.txt
	fgets(info, sizeof(info), proc);
	fputs(info, output3);
	
	fclose(proc);
	
	return 0;
	
}
