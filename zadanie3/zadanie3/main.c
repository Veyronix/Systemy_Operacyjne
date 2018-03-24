#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/times.h>

const int sizeOfBuffor = 1020;
const int sizeOfBlock = 5;

struct rlimit limitTime;
struct rlimit limitSize;

void interpretator(char* text){
    if(text[strlen(text)-1] == '\n') {
        text[strlen(text) - 1] = 0;
    }
    char *tmpAll[5];
    char *tmp= malloc(strlen(text)+1);
    strcpy(tmp, text);
    char *command= strtok(tmp, " ");
    if(command==NULL){
        printf("zle");
        return;
    }
    if(strstr(command, "\n")){
        printf("o nie");
        return;
    }
    int i = 0;
    while((tmpAll[i] = strtok(NULL," "))!= NULL){
        i++;
    }
    struct rusage usage;
    struct timeval end;
    pid_t pid = fork();
    if (pid > 0) {
        int status;

        wait3(&status,0,&usage);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
            printf("\npotomek przegral\n");
            exit(2);
        }
        getrusage(RUSAGE_CHILDREN, &usage);
        end = usage.ru_utime;
        printf("\nStarted at: %ld.%lds\n", end.tv_sec, end.tv_usec);
        end = usage.ru_stime;
        printf("Ended at: %ld.%lds\n", end.tv_sec, end.tv_usec);
    }
    else if( pid == 0){

        if(setrlimit(RLIMIT_CPU,&limitTime)){
            printf("oje");
        }
        if(setrlimit(RLIMIT_AS,&limitSize)) {
            printf("ojej");
        }
        if (execvp(command, tmpAll) == -1) {
           printf("ups");
           exit(1);
        }

    }

}


int main(int argc,char* argv[]) {

    if(argc != 4){
        printf("Bad amount of arguments");
        exit(1);
    }
    FILE *file = fopen(argv[1],"r+");
    if(file == NULL){
        printf("Cant open file");
        exit(1);
    }
    int time = atoi(argv[2]);
    int size = atoi(argv[3]);
    limitSize.rlim_max=size*1048576;
    limitSize.rlim_cur=size*1048576;
    limitTime.rlim_max=time;
    limitTime.rlim_cur=time;
    int amountsOfBlocks = 1;
    int counterOfBlocks = 1;
    char* buffor = (char*) malloc (sizeOfBuffor + 1);
    while(fread(buffor,sizeOfBlock,amountsOfBlocks,file)){
        if(feof(file)){
            interpretator(buffor);
            break;
        }
        if(strstr(buffor, "\n")){
            int index = -1;
            for(int i = 0;i<sizeOfBuffor;i++){
                if(buffor[i]=='\n'){
                    index = i;
                    break;
                }
            }
            if(index != -1) {
                char *function = (char *) malloc(index+1);
                strncpy(function, buffor,index);
                index= (amountsOfBlocks)*sizeOfBlock-index;
                fseek(file, -index+1, SEEK_CUR);
                amountsOfBlocks = 1;
                interpretator(function);
                free(function);
                free(buffor);
                buffor=(char*) malloc (sizeOfBuffor + 1);
                counterOfBlocks--;
            }
        }
        else{
            fseek(file,sizeOfBlock*(-1)*amountsOfBlocks,SEEK_CUR);
            amountsOfBlocks++;
        }
        counterOfBlocks++;
    }


    return 0;
}
