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
    int multiple; 
    int counter;  
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
    op.sem_op = -1; 
    op.sem_flg = 0;
    return semop(semid, &op, 1);
}

int sem_post_op(int semid) {
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1; 
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
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    // configure the size of the shared memory object
    ftruncate(shm_fd, SIZE);

    // memory map the shared memory object
    shared = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    /* create/get a System V semaphore using an ftok key so process2 can find it */
    key_t key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok");
        return 1;
    }
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        return 1;
    }
    /* initialize semaphore to 1 */
    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        return 1;
    }

    /* initialize shared memory safely */
    if (sem_wait_op(semid) == -1) {
        perror("semop wait init");
        return 1;
    }

    shared->multiple = 3;
    shared->counter = 0;

    if (sem_post_op(semid) == -1) {
        perror("semop post init");
        return 1;
    }

    
    pid_t pid;
    /* fork a child process */
    pid = fork();

    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    else if (pid == 0) { /* child process */
        execl("./Part2.5_process2", "part2.5_process2", NULL);
        perror("execl failed");
        exit(1);
    }    

    else { /* parent process */

        while(shared->counter <= 499){
            sem_wait_op(semid);

            if (shared->counter % shared->multiple == 0){
                printf("Process 1: Counter = %d - %d is a multiple of %d\n", shared->counter, shared->counter, shared->multiple); 
                sleep(1);
            }
            else{
                printf("Process 1: Counter = %d\n", shared->counter);
                sleep(1);
            }
            shared->counter++;
            sem_post_op(semid);
        }
        printf("Process 1 Exiting: value of counter is over 500.\n");
        
        wait(NULL);

        // Detach and remove shared memory
        munmap(shared, SIZE);
        shm_unlink(name);

        return 0;
    }
}
