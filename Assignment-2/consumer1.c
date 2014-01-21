/*
Code produced by Andrew DeChamplain 100852795 and Cassandra Perez 100859183
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/shm.h>

#include "common.h"


static sem_t* sem_s;
static sem_t* sem_n;
static sem_t* sem_e;

struct buffer_st* buffer;

int take(void) 
{
	if (!(buffer->active)) 
	{ 
		printf("Buffer is empty. Consumer %d waiting.", getpid());  
		return NULL;
	}

    	int v = buffer->values[buffer->in_index];
    	buffer->in_index = (buffer->in_index + 1) % buff_size;

    	buffer->active--;
    	return v;
}


int main()
{
    	int running = 1;
    	void *shared_memory = (void *)0;
	
	int shmid,num1, num2, num3;

	// allocate space for buffer
	buffer = (struct buffer_st *)malloc(sizeof(struct buffer_st));

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

/* We now make the shared memory accessible to the program. */

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

	
    buffer = (struct buffer_st *)shared_memory;
    while(running) 
	{
        	sem_wait(sem_n);
		sem_wait(sem_s);
		int w = take();		// CS
		sem_post(sem_s);
		sem_post(sem_e);
		printf("SM %d: pid %d takes number %d at position %d.\n", shmid, getpid(), w, buffer->in_index);
		sleep(1);
    	}
}






