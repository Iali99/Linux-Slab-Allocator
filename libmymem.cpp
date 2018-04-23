#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include "libmymem.hpp"

void* mymalloc(unsigned size)
{
    int i=0;

    while((size>>i) != 0)//find appropriate hash value
        i++;
    if(size==1)
        i=2;
    i -= 2;

    int position = i;                       //the bin value
    bucket_locks[position].lock();          //make it thread safe
    header **traverse = &hash_bucket[i];    //to traverse through the list

    while(*traverse !=NULL)
    {
       if((*traverse)->freeobj > 0){        //if there are free objects

           header *ptr = *traverse;
           for(int i = 0,j; i< ptr->totobj/32+1;i++) //find the free object upto 32 bits
               if(ptr->bitmap[i] != 0xffffffff){

                    for(j=0;j<31;j++)  //find the offset within those 32 bits
                        if( (ptr->bitmap[i]&mask[j]) == 0)
                            break;

                    //make changes to metadata
                    ptr->bitmap[i] = ptr->bitmap[i] | mask[j];
                    ptr->freeobj--;
                    int idx = 32*i+j;
                    idx *= ptr->size + 8;
                    bool *ptr1 = (bool*)(ptr+1);
                    ptr1 += idx;
                    *(bool**)ptr1 = (bool*)ptr;
                    bucket_locks[position].unlock(); //Done, unlock the list

                    return (void*)(ptr1+8); //return allocated object
               }
       }
       traverse = &( (*traverse)->next); //if node not free move to next node
    }

    if(*traverse == NULL){ //if no free buckets available
        void *ptr;//create new page
        if((ptr = mmap(NULL, 64*1024, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED){
            perror("mmap");
            exit(1);
        }
        *traverse = (header*)ptr;//link it
        header *temp;
        temp = (header*)ptr;
        //get a new page and initialize header
        int size = (2<<(i+1))+8;
        //meta data changes
        temp->totobj = (64*1024 - sizeof(header))/size;
        temp->freeobj= temp->totobj;
        temp->size   = 2<<(i+1);
        int till = temp->totobj/32;
        for(int i=0;i<till;i++)
            temp->bitmap[i] = 0;
        temp->next = NULL;
        //Done with initialization
        //allocate memory
        bool *allocate =  (bool*)temp+ sizeof(header);
        *(bool**)allocate = (bool*)temp;
        temp->freeobj--;
        temp->bitmap[0] = temp->bitmap[0]| mask[0];
        bucket_locks[position].unlock();
        return (void*)(allocate+8);  //Return memory to calling function
    }
    return NULL;
}

void myfree(void *ptr){

    bool *pt = (bool*)ptr;
    pt-=8; //find header position of node
    header *p = (header*)*(int**)pt;
    int position,i=-2,temp;
    temp = p->size;
    while((temp>>=1)!=0)i++;
    position = i; //find offset of memory to be freed
    bucket_locks[position].lock();   //lock  associated list
    int diff = pt -(bool*)(p+1);
    diff /=  p->size+8;
    p->bitmap[diff>>5] &= ~mask[diff%32]; //make metadata changes
    p->freeobj++;
    //Cleanup peridically, not everytime
    int c = rand()%100;
    void cleanup(int n);
    if(c == 1)
    {
        cleanup_lock.lock();
        cleanup(position);
        cleanup_lock.unlock();
    }
    bucket_locks[position].unlock();
}

void cleanup(int n)
{
    //garbage collection
    header ** traverse = &hash_bucket[n]; //go through n'th list
    while((*traverse) != NULL){
        if ((*traverse)->freeobj == (*traverse)->totobj){ //if empty node found
            //unlink it
            void *temp = (void*)*traverse;
            if((*traverse)->next != NULL)
                *traverse = ((*traverse)->next->next);
            else
                *traverse = NULL;
            //unmap it
            munmap(temp,64*1024);

        }
        else
            traverse = &((*traverse)->next);
        if(*traverse == NULL)
            break;
    }
}
