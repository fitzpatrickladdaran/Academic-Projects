/*
 * lets-talk.c
 * 
 * This program allows two terminals to talk with one another via UDP connection.
 *
 * Name: Fitz Laddaran
 * Date: March 19, 2022
 */

#include "list.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#define max_buffer 4001

void *get_input(void *ptr);
void *retrieve_message(void *ptr);
void *print_message(void *ptr);
void *send_message(void *ptr);

pthread_cond_t l1_check = PTHREAD_COND_INITIALIZER;
pthread_mutex_t l1_lock = PTHREAD_MUTEX_INITIALIZER;
int l1_has_msg = 0;

pthread_cond_t l2_check = PTHREAD_COND_INITIALIZER;
pthread_mutex_t l2_lock = PTHREAD_MUTEX_INITIALIZER;
int l2_has_msg = 0;

pthread_cond_t exit_check = PTHREAD_COND_INITIALIZER;
pthread_mutex_t exit_lock = PTHREAD_MUTEX_INITIALIZER;
int exit_signal = 0;

struct args {  
	List *list;
	int sock;	
	struct addrinfo **rem_user_info;
};

int main(int argc, char *argv[]) {

	// wrong cmd line input
	if (argc != 4) {
		printf("Usage:\n\t./lets-talk <local port> <remote host> <remote port>\n");
		printf("Example:\n\t./lets-talk 3000 192.168.0.513 3001\n");
		printf("\t./lets-talk 3000 some-computer-name 3001\n");
	} 
	
	// correct cmd line input
	else {
	
		List *l1 = List_create();								// list: messages to send
		List *l2 = List_create();								// list: messages received
		
		// reference: http://beej.us/guide/bgnet/html/#man-pages
		// reference: https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
		// reference: https://www.geeksforgeeks.org/udp-server-client-implementation-c/
		// local socket -------------------------------------------
		struct sockaddr_in local_addr;
		int my_sock;
		
		// socket FD
		if ((my_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			printf("Local Socket: socket() failed.\n");
			exit(EXIT_FAILURE);
		}
		
		memset(&local_addr, 0, sizeof(local_addr));
		
		// server info
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = INADDR_ANY;
		local_addr.sin_port = htons(atoi(argv[1]));
		
		if ((bind(my_sock, (const struct sockaddr*) &local_addr, sizeof(local_addr))) == -1) {
			printf("Local Socket: bind() failed.\n");
			exit(EXIT_FAILURE);
		} 
		// --------------------------------------------------------
		
		// remote user info ---------------------------------------
		struct addrinfo hints1, *result1, *server_info1;
		int check;

		memset(&hints1, 0, sizeof(hints1));							// ensure struct is empty
		hints1.ai_family = AF_UNSPEC;								// use IPv4/IPv6
		hints1.ai_socktype = SOCK_DGRAM;							// UDP stream 

		// if remote user info retrieval fails...
		if (check = getaddrinfo(argv[2], argv[3], &hints1, &server_info1) != 0) {
			printf("Remote User: getaddrinfo() failed.\n");
			exit(EXIT_FAILURE);
		}
		// --------------------------------------------------------
		
		// arguments for thread 2: receive messages
		struct args *args_t2 = (struct args*) malloc(sizeof(struct args));
		args_t2 -> list = l2;									// list of messages received
		args_t2 -> sock = my_sock;								// local socket
		args_t2 -> rem_user_info = &server_info1;						// remote user server info
		
		// arguments for thread 4: send messages
		struct args *args_t4 = (struct args*) malloc(sizeof(struct args));
		args_t4 -> list = l1;									// list of messages to send
		args_t4 -> sock = my_sock;								// local socket
		args_t4 -> rem_user_info = &server_info1;						// remote user server info
		
		printf("Welcome to Lets-Talk! Please type your messages now.\n");			// welcome message
		
		pthread_t t1, t2, t3, t4;
		pthread_create(&t1, NULL, get_input, l1); 
		pthread_create(&t2, NULL, retrieve_message, (void*) args_t2);
		pthread_create(&t3, NULL, print_message, l2);
		pthread_create(&t4, NULL, send_message, (void*) args_t4);
		
		// waiting on exit condition from thread 3 or thread 4...
		pthread_mutex_lock(&exit_lock);
		while (exit_signal == 0) {
			pthread_cond_wait(&exit_check, &exit_lock);
		}
		pthread_mutex_unlock(&exit_lock);
		
		pthread_cancel(t1);
		pthread_cancel(t2);
		pthread_cancel(t3);
		pthread_cancel(t4);
		close(my_sock);
		free(args_t2);
		free(args_t4);
		List_free(l1, free);
		List_free(l2, free);

	}

	return 0;
}

// thread 1: waits and accepts user input (messages)
void *get_input(void *ptr) {

	// buffer: user input
	char input[max_buffer];
	
	// get user input
	while (fgets(input, max_buffer, stdin)) {
		
		assert(input != NULL);
		
		// encrypting...
		char *tmp = input;
		while (*tmp != '\0') {
			*tmp = *tmp + 1;
			tmp++;
		}
		
		int check1 = List_append((List*) ptr, input);						// appending inputted string into l1
		
		// if List_append() failed...
		if (check1 != 0) {
			printf("Message Append Failed. Try again!\n");
			exit(EXIT_FAILURE);
		}
		
		// SIGNAL THREAD 4 THAT USER INPUT HAS BEEN APPENDED INTO LIST 1
		// ----------------------------------------------------
		l1_has_msg = 1;
		pthread_cond_signal(&l1_check);
		// ----------------------------------------------------
		
	}

}

// thread 2: receives messages 
void *retrieve_message(void *ptr) {
	
	// buffer: message received
	char msg[max_buffer];	

	while (1) {
		
		// waiting for messages...
		int bytes = recvfrom(((struct args*) ptr) -> sock, msg, max_buffer - 1, 0, (*(((struct args*) ptr) -> rem_user_info)) -> ai_addr, &((*(((struct args*) ptr) -> rem_user_info)) -> ai_addrlen));
		
		// if recvfrom() failed...
		if (bytes == -1) {
			printf("recvfrom() failed. Try again! --> bytes = %d\n", bytes);
			exit(EXIT_FAILURE);
		}
	
		msg[bytes] = '\0';
		
		int check1 = List_append((List*)(((struct args*) ptr) -> list), msg);		// appending message into l2
		
		// if List_append() failed
		if (check1 != 0) {
			printf("Message Append Failed. Try again!\n");
			exit(EXIT_FAILURE);
		}
		
		// SIGNAL THREAD 3 THAT USER INPUT HAS BEEN APPENDED INTO LIST 2
		// ----------------------------------------------------
		l2_has_msg = 1;
		pthread_cond_signal(&l2_check);
		// ----------------------------------------------------

	}
	
}

// thread 3: print messages
void *print_message(void *ptr) {
	
	while (1) {
	
		// WAITING ON THREAD 2 TO APPEND MESSAGE RECEIVED INTO LIST 2
		// ----------------------------------------------------
		pthread_mutex_lock(&l2_lock);
		while (l2_has_msg == 0) {
			pthread_cond_wait(&l2_check, &l2_lock);
		}
		pthread_mutex_unlock(&l2_lock);
		// ----------------------------------------------------
		
		// removing message from l2 (top of list) to be printed
		List_first((List*) ptr);
		char *msg = List_remove((List*) ptr);
		
		// decrypting...
		char *tmp = msg;
		while (*tmp != '\0') {
			*tmp = *tmp - 1;
			tmp++;
		}
		
		printf("%s", msg);									// print message
		fflush(stdout);
		
		if (strcmp(msg, "!exit\n") == 0) {							// exit condition
			exit_signal = 1;
			pthread_cond_signal(&exit_check);
		}
		
		// RESET THREAD 3 LOCK --> WAIT FOR NEW MESSAGE FROM THREAD 2
		// ----------------------------------------------------
		l2_has_msg = 0;
		// ----------------------------------------------------
		
	}
	
}

// thread 4: sends messages
void *send_message(void *ptr) {
	
	while (1) {
	
		// WAITING ON THREAD 1 TO APPEND USER INPUT INTO LIST 1
		// ----------------------------------------------------
		pthread_mutex_lock(&l1_lock);
		while (l1_has_msg == 0) {
			pthread_cond_wait(&l1_check, &l1_lock);
		}
		pthread_mutex_unlock(&l1_lock);
		// ----------------------------------------------------
		
		// removing message from l1 (top of list) to be sent
		List_first(((struct args*) ptr) -> list);
		char *msg = List_remove(((struct args*) ptr) -> list);
		
		// sending message...
		int bytes = sendto(((struct args*) ptr) -> sock, msg, strlen(msg), 0, (*(((struct args*) ptr) -> rem_user_info)) -> ai_addr, (*(((struct args*) ptr) -> rem_user_info)) -> ai_addrlen);
		
		// if sendto() fails...
		if (bytes == -1) {
			printf("Message send: sendto() failed. Try again!\n");
			exit(EXIT_FAILURE);
		}
		
		// decrypting...
		char *tmp = msg;
		while (*tmp != '\0') {
			*tmp = *tmp - 1;
			tmp++;
		}
		
		if (strcmp(msg, "!exit\n") == 0) {							// exit condition
			exit_signal = 1;
			pthread_cond_signal(&exit_check);
		}
		
		// RESET THREAD 4 LOCK --> WAIT FOR NEW USER INPUT FROM THREAD 1
		// ----------------------------------------------------
		l1_has_msg = 0;
		// ----------------------------------------------------
		
	}

}





