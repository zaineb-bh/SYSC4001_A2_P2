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
        int counter = 0;
        pid1 = getpid();


        while(1){
            counter++;
            printf("child: pid = %d\n",pid); /* A */
            printf("child: pid1 = %d\n",pid1); /* B */
            sleep(1);
        }    
    }


    else { /* parent process */
        int counter = 0;
        pid1 = getpid();


        while(1){
            counter++;
            printf("parent: pid = %d\n",pid); /* C */
            printf("parent: pid1 = %d\n",pid1); /* D */
            sleep(1);
        }
    }
    return 0;
}
