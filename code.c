#include <stdio.h>
#include <stdlib.h>

typedef struct header{
   int totojb;//0th byte
   int freeobj;//4th byte
   int size;//8 bytes
   int bitmap[512]; //12
   struct header* next; //2060
   //ends at 2072
} header;

header hash_bucket[12];
int main()
{
    for(int i=0;i<12;i++)
        hash_bucket[i] = NULL;

    //printf("sizeof :%lu, %lu",sizeof(test),sizeof(a));

    return 0;
}
