#include <stdio.h>
#include <unistd.h>  
#include <sys/types.h>
#include <stdlib.h>


int main()
{  
    int counter = 0;
    while(1){
        counter--;
        if (counter % 3 == 0){
            printf("Process 2 Cycle number: %d - %d is a multiple of 3\n", counter, counter); /* C */
            sleep(1);
        }
        else{
            printf("Process 2 Cycle number: %d\n", counter);
            sleep(1);
        }
    }
    return 0;
}