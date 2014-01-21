/*
Code produced by Andrew DeChamplain 100852795 and Cassandra Perez 100859183
*/

#ifndef SYSC_4001_Assignment_2_sem1_h
#define SYSC_4001_Assignment_2_sem1_h

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/sem.h>

#define buff_size 20  // can change to anything

#define semaphore_s "/sem_s"
#define semaphore_n "/sem_n"
#define semaphore_e "/sem_e"

typedef struct buffer_st {
    	int in_index;
    	int out_index;
	int active;
    	int values[buff_size];
} buffer_st;

#endif
