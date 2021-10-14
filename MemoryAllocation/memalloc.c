#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#define MIN_REQ_SIZE 128 //in bytes
#define MAX_REQ_SIZE 2097152 // 2048 kb to bytes
#define MIN_SIZE 32 //in kilobytes
#define MAX_SIZE 32768
int global_method;
struct header
{
    int size;
    struct header* next;
    
};
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(&lock, NULL);
void *chunkptr; // points to the start of memory segment to manage
static int segmentsize; // size of segment to manage

int mem_init(void *chunkpointer, int chunksize, int method){
    //printf("init called\n");
    if(chunksize < MIN_SIZE || chunksize > MAX_SIZE){
        printf("Chunk size is inapppriate!\n"); // if size is less than min size or higher than max size
        return (0);
    }
    chunkptr = chunkpointer;
    chunksize *=1024;
    segmentsize = chunksize;
    global_method = method;

    ((struct header *) chunkptr)->size = 0;
    ((struct header *) chunkptr)->next = NULL;
    
    
    return (0);
}

void *mem_allocate2(int objectsize){
    if(objectsize < MIN_REQ_SIZE || objectsize > MAX_REQ_SIZE){
        printf("Request size is inapppriate!\n"); // if size is less than min size or higher than max size
        return (NULL);
    }

    //printf("allocate called\n");
    switch (global_method){
    case 0: //first fit

        if(objectsize <= segmentsize){
            struct header * currseg = (struct header *) chunkptr;
            while(currseg -> next != NULL){
                // if there is a proper free block between two allocated block
                int p = (char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
                if(objectsize + sizeof(struct header) <= p){
                    struct header *temp = currseg->next;
                    currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg -> size));
                    currseg -> next -> next = temp;
                     currseg -> next -> size = objectsize;
                     
                     return (currseg -> next) + 1;
                }
                currseg = currseg -> next; //iterate if there is no proper free block
            }
            //check if end of the segment has sufficient free space for current request
            if(objectsize + sizeof(struct header) <= (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header)){
                currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg->size));
                currseg -> next -> next = NULL;
                currseg -> next -> size = objectsize;
                
                return (currseg -> next) + 1;
            }
        }
            printf("Allocation cannot be done! objectsize > segmentsize\n" );
            
            return (NULL);
            break;
            
    case 1: //best fit
        if(objectsize < segmentsize){
            
            struct header * best = (struct header *) chunkptr;
            struct header * currseg = (struct header *) chunkptr;
            int min = MAX_REQ_SIZE; //(char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
            int p;
            if(currseg -> next != NULL){
                p = (char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
                int count = 0;
                while(currseg -> next != NULL){
                    
                    if( objectsize + sizeof(struct header) <= p) { // if there is a proper free block between two allocated block
                        count += 1;
                        if(p <= min )//&& objectsize + sizeof(struct header) <= p)
                        {
                        min = p;
                        best = currseg;
                        }
                    }
                    currseg = currseg -> next; //iterate if there is no proper free block
                    p = (char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
                }
                
                if(count == 0){ //add the new
                    if(objectsize + sizeof(struct header) <= (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header)){
                        currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg->size));
                        currseg -> next -> next = NULL;
                        currseg -> next -> size = objectsize;
                        return (currseg -> next) + 1;
                }
                }
                else{ //if a proper free space found btw the block
                    int last = (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header);
                    if(min > last && last > objectsize + sizeof(struct header)) {}
                        best = currseg; // if the free space at the end of chunk is best fit
                        currseg = best;
                        struct header *temp = currseg->next;
                        currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg -> size));
                        currseg -> next -> next = temp;
                        currseg -> next -> size = objectsize;
                        return (currseg -> next) + 1;
                 }
                
                }
            //check if end of the segment has sufficient free space for current request
            if(objectsize + sizeof(struct header) <= (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header)){
                currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg->size));
                currseg -> next -> next = NULL;
                currseg -> next -> size = objectsize;
                return (currseg -> next) + 1;
            }
        }
            printf("Allocation cannot be done! objectsize > segmentsize\n" );
            return (NULL);
            break;
            
    case 2: //worst fit
        if(objectsize < segmentsize){
            
            struct header * worst = (struct header *) chunkptr;
            struct header * currseg = (struct header *) chunkptr;
            int max = MIN_REQ_SIZE; //(char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
            int p;
            if(currseg -> next != NULL){
                p = (char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
                int count = 0;
                while(currseg -> next != NULL){
                    
                    if( objectsize + sizeof(struct header) <= p) { // if there is a proper free block between two allocated block
                        count += 1;
                        if(p >= max )//&& objectsize + sizeof(struct header) <= p)
                        {
                        max = p;
                        worst = currseg;
                        }
                    }
                    currseg = currseg -> next; //iterate if there is no proper free block
                    p = (char *)(currseg->next)- currseg->size - (char *)currseg- sizeof(struct header);
                }
                
                if(count == 0){ //add the new
                    if(objectsize + sizeof(struct header) <= (char *) chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header)){
                        currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg->size));
                        currseg -> next -> next = NULL;
                        currseg -> next -> size = objectsize;
                        return (currseg -> next) + 1;
                }
                }
                else{ //if a proper free space found btw the block
                    int last = (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header);
                    if(max < last && last > objectsize + sizeof(struct header)){
                        worst = currseg; // if the free space at the end of chunk is best fit
                    }
                    currseg = worst;
                    struct header *temp = currseg->next;
                    currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg -> size));
                    currseg -> next -> next = temp;
                     currseg -> next -> size = objectsize;
                     return (currseg -> next) + 1;
                 }
                
                }
            //check if end of the segment has sufficient free space for current request
            if(objectsize + sizeof(struct header) <= (char *)chunkptr + segmentsize - currseg ->size - (char *)currseg - sizeof(struct header)){
                currseg -> next = (struct header *)((char *)currseg + sizeof(struct header) + (currseg->size));
                currseg -> next -> next = NULL;
                currseg -> next -> size = objectsize;
                return (currseg -> next) + 1;
            }
        }
            printf("Allocation cannot be done! objectsize > segmentsize\n" );
            return (NULL);
            break;
    }
    return (NULL);
   
}
void *mem_allocate(int objectsize){
     pthread_mutex_lock(&lock);
     void * ptr = mem_allocate2(objectsize);
     pthread_mutex_unlock(&lock);
     return ptr;
}
void mem_free(void *objectptr){
    pthread_mutex_lock(&lock);
   
    struct header *currseg = chunkptr;
    while( currseg->next && currseg->next != (struct header *)objectptr -1){
        currseg = currseg -> next;
    }
    if(currseg->next){
        struct header *temp = currseg -> next ->next;
        currseg -> next -> size = 0;
        currseg -> next -> next = NULL;
        currseg -> next = temp;
    }
    pthread_mutex_unlock(&lock);
    return;
}

void mem_print(void){
    //printf("print called\n");
    struct header *currseg = chunkptr;
    long int * i = chunkptr;
    if(i == 0){
        printf("Chunk is whole free!\n" );
        return;
    }
    else {
        printf("Implementation alloc: from %p to: %p size: %lu \n",
         currseg,
         currseg + 1,
         sizeof(struct header));
        
        if(!currseg -> next && (segmentsize - sizeof(struct header) > 0)){
            printf("Chunk is whole free = from: %p to: %p size: %lu\n", currseg + 1, 
                (char *)currseg + segmentsize ,
                segmentsize - sizeof(struct header)); //currseg +1 => +1 for the header

        }
        else {
            
            while(currseg){
                //sleep(1);
                if(currseg -> size != 0){

                    printf("Alloc from %p to: %p, block + header size: %lu\n", currseg ,
                        (char *)currseg + currseg-> size ,
                        currseg -> size + sizeof(struct header));
                    //printf("char temp %p\n", currseg );
                }
                
                if(currseg -> next && ((char *)(currseg -> next) - currseg -> size - (char*)currseg- sizeof(struct header) > 0)){
                    printf("Free  from %p to: %p,                size: %lu\n", currseg -> size + (char *)currseg + sizeof(struct header),
                        currseg -> next,
                        (char *)(currseg -> next) - currseg->size - (char *)currseg -sizeof(struct header));
                }
                if((!currseg -> next) && ((char *)(chunkptr) + segmentsize - currseg-> size
                    - (char * )currseg - sizeof(struct header) > 0 )){
                    printf("Free  from %p to: %p,                size: %lu \n", currseg -> size + (char *)currseg + sizeof(struct header),
                        (char *)chunkptr + segmentsize,
                        ((char *)(chunkptr) + segmentsize - currseg->size - (char *)currseg - sizeof(struct header)));
                }
                currseg = currseg -> next;
                
            }
        }

    }
   

    return ;
}


