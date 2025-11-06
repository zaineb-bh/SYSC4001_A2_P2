#include <stdio.h>
#include <unistd.h>  
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>

struct sharedMemory {
    int multiple; // the multiple number
    int counter;  // shared counter
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
#if defined(__linux__)
    struct seminfo *__buf;
#endif
};

int sem_wait_op(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1; /* P */
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_post_op(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1; /* V */
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

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

    key_t key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok (child)");
        return 1;
    }
    int semid = semget(key, 1, 0666);
    if (semid == -1) {
        perror("semget (child)");
        return 1;
    }

    int printed = -1;
    while(shared->counter <= 99){
        sem_wait_op(semid);

        if(shared->counter != printed){
            printf("Process 2: Counter = %d, waiting for value of counter to exceed 100\n", shared->counter);
            printed = shared->counter;
            sleep(1);
        }
        sem_post_op(semid);
    }

    while(shared->counter <= 499){
        sem_wait_op(semid);

        if (shared->counter % shared->multiple == 0){
            printf("Process 2: Counter = %d - %d is a multiple of %d\n", shared->counter, shared->counter, shared->multiple);
            sleep(1);
        }
        else{
            printf("Process 2: Counter = %d\n", shared->counter);
            sleep(1);
        }
        sem_post_op(semid);
    }
    printf("Process 2 Exiting: value of counter is over 500\n");

    munmap(shared, SIZE);
    
    return 0;
}
