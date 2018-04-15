#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("Not enought arguments");
        exit(1);
    }
    
    printf("My Pid is: %d\n", getpid());
    ssize_t read = 0;
    size_t size = 0;
    char *buffer = NULL;

    int N = atoi(argv[2]);
    if(N < 0){
        printf("Invalid number");
        exit(0);
    }

    FILE *fifo;
    if ((fifo = fopen(argv[1], "w")) == NULL)
    {
        printf("cant open fifo");
        exit(2);
    }
    srand(time(NULL));
    for(int i=0; i < N; i++){
        char line[128];
        FILE *date = popen("date", "r");
        if (date == NULL)
        {
            printf("Failed to open read  date");
            return 3;
        }
        if ((read = getline(&buffer, &size, date)) > 0){  
            sprintf(line, "<%d> %s",getpid(),buffer);
            fwrite(line, 1, strlen(line), fifo);
        }else{
            printf("failed to get date");
            exit(2);
        }
        pclose(date);
        sleep(rand() % 5);
    }

    return 0;
}