#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>

typedef struct header{
   int totobj;//0th byte
   int freeobj;//4th byte
   int size;//8 bytes
   int bitmap[256]; //12
   struct header* next; //1036
   //ends at 1048
} header;

header *hash_bucket[12];
void* mymalloc(unsigned size);
void myfree(void *ptr);

int mask[32];

int main()
{
    printf("Initializing\n");
    //initialize
    for(int i=0;i<12;i++)
        hash_bucket[i] = NULL;
    for(int i=31,j=1;i>=0;i--,j<<=1)
        mask[i]=j;
    //done initializing
    int *ptr[50000];
    printf("starting off\n");
    for(int i =0 ;i<50000;i++){
        ptr[i] = (int*)mymalloc(10);
        if(i%100==0)
            printf("%d\n",i);

    }
    printf("FREEING\n");
    myfree(ptr[49999]);
    printf("DONE WITH ONE\n");
    for(int i =0 ;i<50000-1;i++){

        myfree(ptr[i]);
        if(i%100==0)
            printf("%d\n",i);
    }

    //int *ptr = (int*)mymalloc(10);
    //ptr = (int*)mymalloc(10);
    //printf("%ld",ptr-ptr1);
    printf("Done\n");


    return 0;
}

void* mymalloc(unsigned size){
    int i=0;
    while((size>>i) != 0)//find appropriate hash value
        i++;
    if(size==1)
        i=2;
    i -= 2;
    //printf("i :%d\n",i);

    header **traverse = &hash_bucket[i];
    while(*traverse !=NULL){
        printf("FREE OBJ:%d\n",(*traverse )-> freeobj);
       if((*traverse)->freeobj <= 0){
           //printf("HI\n");
           //printf("Traverse:%p",traverse);
       }

       else{
           //printf("SECOND\n");
           header *ptr = *traverse;
           for(int i = 0,j; i< ptr->totobj/32+1;i++)
               if(ptr->bitmap[i] != 0xffffffff){ //printf("BLAH");
                    for(j=0;j<31;j++)
                        if( (ptr->bitmap[i]&mask[j]) == 0)
                        { //printf("1BITMAP%x\n",ptr->bitmap[i]);
                            break;}
                    ptr->bitmap[i] = ptr->bitmap[i] | mask[j];
                    //printf("2BITMAP%x\n",ptr->bitmap[i]);
                    ptr->freeobj--;
                    int idx = 32*i+j;
                    idx *= ptr->size + 8;
                    //printf("SIZE:%d",ptr->size);
                    ///printf("idx :%d\n",idx);
                    bool *ptr1 = (bool*)(ptr+1);
                    ptr1 += idx;
                    *(bool**)ptr1 = (bool*)ptr;
                    printf("SIZE : %d\n",((header*)ptr1)->size);
                    return (void*)(ptr1+8);
               }
       }
           traverse = &( (*traverse)->next);
       //printf("INF");
    }
    if(*traverse == NULL){
        void *ptr;//create new page
        printf("FIRST\n");
        if((ptr = mmap(NULL, 64*1024, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)) == MAP_FAILED){
            perror("mmap");
            exit(1);
        }
        *traverse = (header*)ptr;//link it
        header *temp;
        temp = (header*)ptr;
        //get a new page and initialize header
        int size = (2<<(i+1))+8;
        temp->totobj = (64*1024 - sizeof(header))/size;
        temp->freeobj= temp->totobj;
        temp->size   = 2<<(i+1);
        int till = temp->totobj/32;
        for(int i=0;i<till;i++)
            temp->bitmap[i] = 0;
        temp->next = NULL;
        //Done with initialization
        //allocate memory
        bool *allocate = sizeof(header) + (bool*)temp;
        *(bool**)allocate = (bool*)temp;
        temp->freeobj--;
        temp->bitmap[0] = temp->bitmap[0]| mask[0];

        return (void*)(allocate+8);

    }
    return NULL;

}

void myfree(void *ptr){
    bool *pt = (bool*)ptr;
    pt-=8;
    header *p = (header*)pt;
    int diff = (bool*)(p+1)-pt;
    printf("HIHI %d\n",p->size);
    diff /=  p->size;
    p->bitmap[diff>>5] &= ~mask[diff%32];
    p->freeobj++;

}
