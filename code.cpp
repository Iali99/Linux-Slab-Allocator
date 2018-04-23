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

header *hash_bucket[12]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
void* mymalloc(unsigned size);
void myfree(void *ptr);

int mask[32] = {0x80000000,0x40000000,0x20000000,0x10000000,0x08000000,0x04000000,0x02000000,0x01000000,0x00800000,0x00400000,0x00200000,0x00100000,0x00080000,0x00040000,0x00020000,0x00010000,0x00008000,0x00004000,0x00002000,0x00001000,0x00000800,0x00000400,0x00000200,0x00000100,0x00000080,0x00000040,0x00000020,0x00000010,0x00000008,0x00000004,0x00000002,0x00000001};

//int main()
//{
//    printf("Initializing\n");
//    //initialize
//    for(int i=0;i<12;i++)
//        hash_bucket[i] = NULL;
//    for(int i=31,j=1;i>=0;i--,j<<=1)
//        mask[i]=j;
//    //done initializing
//    int *ptr[5500];
//    printf("starting off\n");
//    for(int i =0 ;i<5500;i++){
//        ptr[i] = (int*)mymalloc(100);
//        if(i%100==0)
//            printf("%d\n",i);
//
//    }
//    printf("FREEING\n");
//    for(int i =0 ;i<5500;i++){
//
//        printf("%d  ",i);
//        myfree((void*)ptr[i]);
//    }
//
//    //int *ptr = (int*)mymalloc(10);
//    //ptr = (int*)mymalloc(10);
//    //printf("%ld",ptr-ptr1);
//    printf("Done\n");
//
//
//    return 0;
//}

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
                    printf("SIZE : %d\n",((header*)(*(int**)ptr1))->size);
                    printf("SIZE2: %d\n",ptr->size);
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
        bool *allocate =  (bool*)temp+ sizeof(header);
        *(bool**)allocate = (bool*)temp;
        temp->freeobj--;
        temp->bitmap[0] = temp->bitmap[0]| mask[0];
        printf("SIZEBLALKDJFLKJAKL JLKDAJF LKJDLFJ LKJ : %d\n",((header*)(*(int**)allocate))->size);

        return (void*)(allocate+8);

    }
    return NULL;

}

void myfree(void *ptr){
    bool *pt = (bool*)ptr;
    pt-=8;
    header *p = (header*)*(int**)pt;
    int diff = pt -(bool*)(p+1);
    diff /=  p->size+8;
    printf("HIHI %d    %d\n",p->size,diff);
    p->bitmap[diff>>5] &= ~mask[diff%32];
    p->freeobj++;

}
