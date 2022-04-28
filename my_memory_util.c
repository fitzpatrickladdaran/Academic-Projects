/*
 * my_memory_util.c
 * 
 * This program prints the memory utilization of the computer system.
 *
 * Name: Fitz Laddaran
 * Date: January 27, 2022
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function: converts the value of the memory size from a string to an integer
// input: a string
// return value: an integer representing the value of the memory size
long get_memory_size(char *line) {
	
	long kB = 0;

	for (int i = 0; i < strlen(line); i++) {
	
		if(isdigit(line[i])) {
			char* ptr = &line[i];
			kB = strtol(ptr, &ptr, 10);
			break;
		}

	}
	
	return kB;
	
}


// function: calculates the value for memory utilization
// input: 5 long variables, each represent a value used in calculating memory utilization
// return value: a float representing the value of memory utilization
float memory_utilization(long total, long free, long buffers, long cached, long slab) {
	
	float diff = total - free - buffers - cached - slab;
	float mem_util = (diff / total) * 100;
	return mem_util;
	
}


int main() {

	char info[512];
	long total, free, buffers, cached, slab;
	float mem_util;
	int line = 1;
	
	FILE *fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) return 0;
	
	FILE *meminfo = fopen("meminfo.txt", "w");
	if (meminfo == NULL) return 0;
	
	// reading every line in /proc/meminfo
	while (fgets(info, sizeof(info), fp)) {
	
		// directing every outputted line into meminfo.txt
		fputs(info, meminfo);
		
		// search for "total" value
		if (line == 1) {
			total = get_memory_size(info);
		} 
		
		// search for "free" value
		else if (line == 2) {
			free = get_memory_size(info);
		} 
		
		// search for "buffers" value
		else if (line == 4) {
			buffers = get_memory_size(info);
		} 
		
		// search for "cached" value
		else if (line == 5) {
			cached = get_memory_size(info);
		} 
		
		// search for "slab" value
		else if (line == 23) {
			slab = get_memory_size(info);
		}
		
		line++;

	}
	
	fclose(meminfo);
	fclose(fp);
	
	// calculating memory utilization
	mem_util = memory_utilization(total, free, buffers, cached, slab);
	
	FILE *output2 = fopen("output2.txt", "w");
	if (output2 == NULL) return 0;
	
	fprintf(output2, "%.2f", mem_util);
	fclose(output2);
	
	return 0;
	
}
