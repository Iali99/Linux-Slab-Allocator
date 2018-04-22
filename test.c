#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{

    bool *a,*b;
    double d=4;
    b = (bool*)&d;
    int c=3;
    a = (bool*)&c;
    *(bool**)b = (int*)a;
    printf("%d= a, %d=b",*(int*)a,**(int**)b);


    return 0;
}

