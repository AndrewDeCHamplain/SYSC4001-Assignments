/*
Code Produced by Andrew DeChamplain 100852795 and Cassasndra Perez 100859183.

The Monitor connects to the Controller via FIFO by sending "connect" which then waits for the Controller
to send "Start". Once told to start, the child process continuously sends a random number between 60-100 
to the controller where it is displayed and the Monitors number that sent it.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "monitor.h"
#include <ctype.h>

#define MAX_TEXT 512

struct my_msg_st {
    long int my_msg_type;
    char some_text[MAX_TEXT];
};

int main()
{
  	int server_fifo_fd, client_fifo_fd;
   	struct data_to_pass_st my_data;
	int read_res, running, running1, HR, r, msgid, delay;
	char client_fifo[256];
	char *tmp_data2;
	char buffer[BUFSIZ];
	struct my_msg_st more_data;
    	long int msg_to_receive = 0;
	pid_t pid;
	
	
	// connect to message queue
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	// check for errors connecting
	if (msgid == -1) {
        	fprintf(stderr, "msgget failed with error: %d\n", errno);
        	exit(EXIT_FAILURE);
    	}

	// allocate space for a temp char to making comparisons
	tmp_data2 = (char *)malloc(19);
	running = 1;
	running1 = 1;

	// open fifo to connect to Controller
    	server_fifo_fd = open(SERVER_FIFO_NAME, O_WRONLY);
	// check for errors creating fifo
    	if (server_fifo_fd == -1) {
        	fprintf(stderr, "Sorry, no server\n");
        	exit(EXIT_FAILURE);
    	}

	// get the Monitors pid (to be used as Monitors name)
    	my_data.client_pid = getpid();
    	sprintf(client_fifo, CLIENT_FIFO_NAME, my_data.client_pid);
    	if (mkfifo(client_fifo, 0777) == -1) {
        	fprintf(stderr, "Sorry, can't make %s\n", client_fifo);
        	exit(EXIT_FAILURE);
    	}

	// send "Connect" as well as the Monitors pid to the Controller
        sprintf(my_data.some_data, "Connect %d", my_data.client_pid); 
        write(server_fifo_fd, &my_data, sizeof(my_data));
        client_fifo_fd = open(client_fifo, O_RDONLY);
	printf("connect sent\n");

	// wait for a response
	read_res = read(client_fifo_fd, &my_data, sizeof(my_data));
        if (read_res > 0) {
		sprintf(tmp_data2, "Start %d", my_data.client_pid); 
		// check if response is "Connect" + pid, if so then create child and begin sending random heatrates 		
		if (strcmp(my_data.some_data, tmp_data2) == 0){
			printf("received start\n");
			printf("started\n");
			pid = fork();

			while (running){
				switch (pid){	
				case -1: 
					perror("Fork Failed");
					exit(1);
				case 0:	 // child process
					while (running1){
        					more_data.my_msg_type = 1;
						sprintf(more_data.some_text, "Monitor %d has a heartrate of %d\n", my_data.client_pid, (rand() % 41) + 60);
        					if (msgsnd(msgid, (void *)&more_data, MAX_TEXT, 0) == -1) {
            						fprintf(stderr, "msgsnd failed\n");
            						exit(EXIT_FAILURE);
        					}
						printf("Sending HR\n");
						// recieve data from Controller 
						if (msgrcv(msgid, (void *)&more_data, MAX_TEXT, msg_to_receive, 0) == -1) {
            						fprintf(stderr, "msgrcv failed with error: %d\n", errno);
            						exit(EXIT_FAILURE);
        					}
						// check to see if recieved data is "ACK" and if true print to user.
						if (strcmp(more_data.some_text, "ACK\n") == 0){
							printf("ACK\n");
							sprintf(more_data.some_text, "This will never print\n");
						
						}
						// if not "ACK" then it has lost connection with Controller and needs to terminate
						else{
							printf("\nConnect with Controller lost\n");
							running1 = 0;
							close(client_fifo_fd);
							unlink(CLIENT_FIFO_NAME);
							printf("Fifo closed\n");
							kill(pid, SIGKILL);
							exit(EXIT_SUCCESS);
						}
						sleep(2);
					}
				default:
					sleep(1);
				}	
			}	
		}
    	}
}

