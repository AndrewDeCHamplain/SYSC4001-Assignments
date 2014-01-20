/*
Code produced by Andrew DeChamplain 100852795 and Cassandra Perez 100859183
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>

#include "monitor.h"
#include <ctype.h>
#define MAX_TEXT 512

int server_fifo_fd, msgid;

struct my_msg_st {
    	long int my_msg_type;
   	char some_text[BUFSIZ];
};

void ouch(int sig)
{
    	printf("\nCtrl pressed\n");
	// close fifo	
	close(server_fifo_fd);
    	unlink(SERVER_FIFO_NAME);
	printf("Closing message queue...\n");
	sleep(3);
	
	//close the message queue
	msgctl(msgid, IPC_RMID, 0);
        printf("Message queue closed\n");
    	exit(EXIT_SUCCESS);
}

int main()
{
    	int server_fifo_fd, client_fifo_fd;
    	struct data_to_pass_st my_data;
    	int read_res, running, running1, delay;
    	char client_fifo[256];
    	char *tmp_data;
    	struct my_msg_st more_data;
    	long int msg_to_receive = 0;
	pid_t pid;

	// allocate sapce for a temp char for comparisons
	tmp_data = (char *)malloc(19);

	running = 1;
	running1 = 1;
	// open fifo
    	mkfifo(SERVER_FIFO_NAME, 0777);
    	server_fifo_fd = open(SERVER_FIFO_NAME, O_RDONLY);
	// if fifo was not made, return an error
    	if (server_fifo_fd == -1) {	
        	fprintf(stderr, "Server fifo failure\n");
        	exit(EXIT_FAILURE);
    	}

	// create message q
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	// check for errors
	if (msgid == -1) {	
        	fprintf(stderr, "msgget failed with error: %d\n", errno);
        	exit(EXIT_FAILURE);
   	}
	
	// create child and parent, the child loops and prints the heartrates of Monitors and their id's when recieved and 
	// sends a "ACK" message.
	// the parent continues to wait for more Monitors to send "Connect" to it and then returns "start".
	pid = fork();

	switch(pid){
	case -1:
		perror("Fork Failed");
		exit(1);
	case 0:
		while (running1){
			sprintf(more_data.some_text, "0");
			// recieve data from Monitors
			if (msgrcv(msgid, (void *)&more_data, BUFSIZ, msg_to_receive, 0) == -1) {
            			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            			exit(EXIT_FAILURE);
        		}
			// prints data unless it is "ACK" or "0"
			if ((strcmp, "0", more_data.some_text) != 0 | (strcmp, "ACK\n", more_data.some_text) != 0){
				printf("%s", more_data.some_text);
				sprintf(more_data.some_text, "ACK\n");
			}
			// send the "ACK" back to Monitor
			if(msgsnd(msgid, (void *)&more_data, MAX_TEXT, 0) == -1) {
            			fprintf(stderr, "msgsnd failed\n");
            			exit(EXIT_FAILURE);
        		}			
		}
	default:
    		while (running){
        		read_res = read(server_fifo_fd, &my_data, sizeof(my_data));
			// wait for some data to be sent from monitors
        		if (read_res > 0) {	
				printf("read() true\n");
			
				// check if data received is "connect", if true, send "start"
				sprintf(tmp_data, "Connect %d", my_data.client_pid);
				if (strcmp(my_data.some_data,tmp_data) == 0){	
					printf("received connect\n");
					sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
					client_fifo_fd = open(client_fifo, O_WRONLY); 
            				if (client_fifo_fd != -1) {
					
						sprintf(my_data.some_data, "Start %d", my_data.client_pid);
                				write(client_fifo_fd, &my_data, sizeof(my_data));
						printf("sent start\n");
                				close(client_fifo_fd);
            				}
					else {
						printf("Error: client_fifo_fd did not open.\n");
					}
				}
        		}
			(void) signal(SIGINT, ouch);
    		}
	}
}

