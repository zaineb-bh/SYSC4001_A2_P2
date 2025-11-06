#include <stdio.h>
#include <unistd.h>  
#include <sys/types.h>
#include <stdlib.h>


int main()
{  
    pid_t pid;
    pid_t pid1;
    /* fork a child process */
    pid = fork();


    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }


    else if (pid == 0) { /* child process */
        execl("./Part2.2_process2", "part2.2_process2", NULL); 
        perror("execl failed");
        exit(1);
    }    

    else { /* parent process */
        int counter = 0;
        pid1 = getpid();
        while(1){
            counter++;
            if (counter % 3 == 0){
                printf("Process 1 Cycle number: %d - %d is a multiple of 3\n", counter, counter); /* C */
                sleep(1);
            }
            else{
                printf("Process 1 Cycle number: %d\n", counter);
                sleep(1);
            }
        }
    return 0;
    }
}