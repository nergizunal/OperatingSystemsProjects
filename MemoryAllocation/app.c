#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "memalloc.h"
#define THREADS 10
#define ALLOCNUM 6
#define THR_ALLOCSIZE
static int req;
static void *x[ALLOCNUM]; // object pointers
void* func(void *);

int main(int argc, char *argv[])
{
	void *chunkptr;
	void *endptr;
	char *charptr;
	int ret;
	int i;
	int size;
	

	pthread_t tid[THREADS];
	if (argc != 2) {
		printf("usage: app <size in KB>\n");
		exit(1);
	}
	size = atoi(argv[1]); // unit is in KB
	// allocate a chunk
	
	chunkptr = sbrk(0); // end of data segment
	sbrk(size * 1024); // extend data segment by indicated amount (bytes)
	endptr = sbrk(0); // new end of data segment
	printf("chunkstart=%lx, chunkend=%lx, chunksize=%lu bytes\n",
	(unsigned long)chunkptr,
	(unsigned long)endptr, (unsigned long)(endptr - chunkptr));
	//test the chunk
	printf("---starting testing chunk\n");
	charptr = (char *)chunkptr;
	for (i = 0; i < size; ++i)
	charptr[i] = 0;
	printf("---chunk test ended - success\n");
	ret = mem_init(chunkptr, size, FIRST_FIT);
	if (ret == -1) {
		printf("could not initialize \n");
		exit(1);

	}

	req = size * 64;
	for(int i = 0; i < ALLOCNUM; i++){
		x[i] = (int *) mem_allocate(req);
		if(x[i] == NULL){
			printf("Cannot allocate.\n" );
			exit(1);
		}
	}
	mem_free(x[1]);
	mem_free(x[2]);
	mem_free(x[4]);
	 
	req = size * 32; // 1024/ 4 = 256, with header 63 allocation can be done
	
	for(int i = 0; i < THREADS; i++){
		pthread_create(&tid[i], NULL, func, NULL);

	}

	for(int i = 0; i < THREADS; i++){
		pthread_join(tid[i], NULL);
	}
	

	mem_print();
	return 0;
}

void* func (void *arg){
	
	//int a = *((int*) arg);
	//printf("%d\n",a );
	void *arr = mem_allocate(req);
	
	if(arr == NULL){
		printf("Cannot Allocate. \n" );
		pthread_exit(0);
		
	}
	
	pthread_exit(0);
	return arr;
}
