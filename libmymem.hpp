#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdbool.h>
#include <mutex>

using namespace std;
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

mutex bucket_locks[12];
mutex cleanup_lock;
unsigned int mask[32] = {0x80000000,0x40000000,0x20000000,0x10000000,0x08000000,0x04000000,0x02000000,0x01000000,0x00800000,0x00400000,0x00200000,0x00100000,0x00080000,0x00040000,0x00020000,0x00010000,0x00008000,0x00004000,0x00002000,0x00001000,0x00000800,0x00000400,0x00000200,0x00000100,0x00000080,0x00000040,0x00000020,0x00000010,0x00000008,0x00000004,0x00000002,0x00000001};
