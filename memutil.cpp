#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libmymem.hpp"
int main()
{

    srand(19235);
    int *ptr[1000];
    int arr[1000];
    for(int i=0;i<1000;arr[i++] = i);
    for(int i = 0;i<1000;i++)
    {
        unsigned int s = rand()%8192 + 1;
        printf ("random request size = %d\n",s);
        ptr[i] = (int*) mymalloc(s);
        *(int*)ptr[i]=arr[i];
    }
    for(int i=0;i<1000;i++){
        printf("i :%d   value:%d\n",i,*ptr[i]);
        myfree(ptr[i]);
    }
    printf("DONE\n");

    return 0;
}

