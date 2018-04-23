#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "code.h"
int main()
{
    int *ptr = (int*) mymalloc(10);
    myfree(ptr);


    return 0;
}

