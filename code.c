#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct header{
   int totobj;//0th byte
   int freeobj;//4th byte
   int size;//8 bytes
   int bitmap[512]; //12
   struct header* next; //2060
   //ends at 2072
} header;

header *hash_bucket[12];
void* mymalloc(unsigned size);
void* myfree(void *ptr);

int main()
{
    for(int i=0;i<12;i++)
        hash_bucket[i] = NULL;


    return 0;
}

void* mymalloc(unsigned size){
    int i=0;
    while((size>>i) != 0)//find appropriate hash value
        i++;
    if(size==1)
        i=2;
    i -= 2;

    header **traverse = &hash_bucket[i];
    while(*traverse!=NULL){

    }
    if(*traverse == NULL){
        void *ptr;//create new page
        if((ptr = mmap(NULL, 64*1024, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED){
            perror("mmap");
            exit(1);
        }
        *traverse = (header*)ptr;//link it
        header *temp;
        temp = (header*)ptr;
        //get a new page and initialize header
        temp->totobj = (64*1024 - 2072)/(2<<(i+1));
        temp->freeobj= temp->totobj;
        temp->size   = 2<<(i+1);
        int till = temp->totobj/32;
        for(int i=0;i<till;i++)
            temp->bitmap[i] = 0;
        temp->next = NULL;
        //Done with initialization
        //allocate memory



    }

}



