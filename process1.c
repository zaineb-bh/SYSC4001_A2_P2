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
    int multiple; 
    int counter;  
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
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    // configure the size of the shared memory object
    ftruncate(shm_fd, SIZE);

    // memory map the shared memory object
    shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    shared->multiple = 3;
    shared->counter = 0;
    
    pid_t pid;
    /* fork a child process */
    pid = fork();

    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    else if (pid == 0) { /* child process */
        execl("./process2", "process2", NULL);
        perror("execl failed");
        exit(1);
    }    

    else { /* parent process */

        while(shared->counter <= 500){
            if (shared->counter % shared->multiple == 0){
                printf("Process 1: Counter = %d - %d is a multiple of 3\n", shared->counter, shared->counter); /* C */
                sleep(1);
            }
            else{
                printf("Process 1: Counter = %d\n", shared->counter);
                sleep(1);
            }
            shared->counter++;
           
        }
        printf("Process 1 Exiting: value of counter is over 500.\n");
        
        wait(NULL);

        // Detach and remove shared memory
        munmap(shared, SIZE);
        shm_unlink(name);

        return 0;
    }
}
