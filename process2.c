#include <stdio.h>
#include <unistd.h>  
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

struct sharedMemory {
    int multiple; // the multiple number
    int counter;  // shared counter
};

int main()
{  
   //size (in bytes) of shared memory object 
   const int SIZE = 4096;  
   //name of the shared memory object             
   const char *name = "OS";      

   //shared memory file descriptor
   int shm_fd;
   // pointer to shared memory obect                      
   struct sharedMemory *shared;      

   // create the shared memory object
   shm_fd = shm_open(name, O_RDWR, 0666);

   // memory map the shared memory object
   shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int printed = -1;
    while(shared->counter <= 100){
        if(shared->counter != printed){
            printf("Process 2: Counter = %d, waiting for value of counter to exceed 100\n", shared->counter);
            printed = shared->counter;
            sleep(1);
        }
    }

    while(shared->counter <= 500){
        if (shared->counter % shared->multiple == 0){
            printf("Process 2: Counter = %d - %d is a multiple of 3\n", shared->counter, shared->counter);
            sleep(1);
        }
        else{
            printf("Process 2: Counter = %d\n", shared->counter);
            sleep(1);
        }
    }
    printf("Process 2 Exiting: value of counter is over 500\n");

    munmap(shared, SIZE);
    

    return 0;
}
