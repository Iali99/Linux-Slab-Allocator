#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libmymem.hpp"
#include <thread>
#include <vector>

void threadFunction(int n);

int main(int argc, char *argv[])
{
    int t,n;
    if (argc != 3){
        printf("USAGE memutil #threads #iterations\n");
        return 1;
    }
    t = atoi(argv[1]);
    n = atoi(argv[2]);
    std::vector<std::thread> threads;
    printf("Starting Threads\n");
    for(int i = 0;i<t;i++)
    {
        threads.emplace_back(threadFunction,n);
    }
    for(auto& th: threads)
    {
        th.join();
    }
    printf("Done with threads\n");
    return 0;
}

void threadFunction(int n)
{
    srand(12621);
    for(int i =0;i<n;i++)
    {
       unsigned int s = rand()%8192 +1;
       char * ptr = (char *)mymalloc(s);
       char * temp = ptr;
       for(int i = 0;i<s;i++)
       {
           *temp = 'a';
           temp++;
       }
       usleep(100);
       myfree((void*)ptr);
    }
}
