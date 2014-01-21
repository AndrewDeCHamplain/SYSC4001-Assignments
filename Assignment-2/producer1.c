/*
Code produced by Andrew DeChamplain 100852795 and Cassandra Perez 100859183
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <semaphore.h>
#include <sys/shm.h>

#include "common.h"

static sem_t* sem_s;
static sem_t* sem_n;
static sem_t* sem_e;

struct buffer_st* buffer;

void append(int value) {

	buffer->values[buffer->out_index] = value;
    	buffer->out_index = (buffer->out_index + 1) % buff_size;

    	if (buffer->active < buff_size){
    		buffer->active++;
    	} else {
        	printf("Shouldn't get here, go away.( ͡° ͜ʖ ͡°)\n");
		sleep(1);
    	}
}

int main(int argc, char *argv[])
{
	int shmid, v, num1, num2, num3;
    	int running = 1;
    	void *shared_memory = (void *)0;
   	
	shmdt(buffer);
	shmdt(shared_memory);

	// allocate space for buffer
	buffer = (struct buffer_st *)malloc(sizeof(buffer_st));
	// allocate space for semaphores
	sem_s = (sem_t *)malloc(sizeof(sem_t));
	sem_n = (sem_t *)malloc(sizeof(sem_t));
	sem_e = (sem_t *)malloc(sizeof(sem_t));

    shmid = shmget((key_t)1234, sizeof(struct buffer_st), 0666 | IPC_CREAT);
	shmid = 192387362;
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

	// clear sems
	sem_unlink(sem_s);
	sem_unlink(sem_n);
	sem_unlink(sem_e);

	// initiallize the semaphores
	sem_s = sem_open(semaphore_s, O_CREAT, 0777, 1);
    	if (SEM_FAILED == sem_s) {
        	printf("Failed to create semaphore with errno=%d\n", errno);
        	exit(0);
    	}
	sem_n = sem_open(semaphore_n, O_CREAT, 0777, 0);
    	if (SEM_FAILED == sem_n) {
        	printf("Failed to create semaphore with errno=%d\n", errno);
        	exit(0);
    	}
	sem_e = sem_open(semaphore_e, O_CREAT, 0777, buff_size);
    	if (SEM_FAILED == sem_e) {
        	printf("Failed to create semaphore with errno=%d\n", errno);
        	exit(0);
    	}

	// make buffer in shared mem
    buffer = (struct buffer_st *)shared_memory;
	buffer->active = 0;
	buffer->in_index = 0;
    	buffer->out_index = 0;

    while(running) {
		// make random v, based on argc. When argc==1, range is 1-100, argc==2, range is 101-200, etc...
		v = (rand() % 100)+((argc*100)-99);
		sem_wait(sem_e);
		sem_wait(sem_s);
		append(v);	// CS
		sem_post(sem_s);
		sem_post(sem_n);
		printf("SM %d: pid %d puts number %d at position %d.\n", shmid, getpid(), v, buffer->out_index);
		sleep(1);
    }
	if (shmdt(shared_memory) == -1) {
        	fprintf(stderr, "shmdt failed\n");
        	exit(EXIT_FAILURE);
    	}
    exit(EXIT_SUCCESS);
}












