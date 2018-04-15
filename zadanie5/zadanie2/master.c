#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define WRITE_END 1
#define READ_END 0


int main(int argc,char* argv[]) {
    if(argc != 2){
        printf("Bad amount of arguments");
        exit(1);
    }
    if (mkfifo(argv[1], 0666) == -1)
    {
        perror("cant create FIFO");
        return 2;
    }
      
    char line [ 128 ];
    while(1){
        FILE *fifo;
        if ((fifo = fopen(argv[1], "r")) == NULL)
        {
            perror("Failed to open file.");
            exit(2);
        }
        while (fgets ( line, sizeof line, fifo ) > 0)
        {
            printf("%s", line);
            
        }
    }
    return 0;


}