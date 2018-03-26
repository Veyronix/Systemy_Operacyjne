#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/wait.h>



struct rlimit limitTime;
struct rlimit limitSize;

void interpretator(char* text){
    printf("\nLaunch: %s\n",text);
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
        printf("\nU time: %ld.%lds\n", end.tv_sec, end.tv_usec);
        end = usage.ru_stime;
        printf("S time: %ld.%lds\n\n", end.tv_sec, end.tv_usec);
        if(WIFSIGNALED(status)){
            printf("terminated %d" , WTERMSIG(status));
            exit(WTERMSIG(status));

        }
        return;
    }
    else if( pid == 0){

        if(setrlimit(RLIMIT_CPU,&limitTime)){
            printf("oje");
        }
        if(setrlimit(RLIMIT_AS,&limitSize)) {
            printf("ojej");
        }
        tmpAll[0]=command;
        printf("%s",command);

        if(execv(command,tmpAll)== -1){
            if (execvp(command, tmpAll) == -1) {
                printf("ups");
                exit(1);
            }
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
    char line [ 128 ];
    while ( fgets ( line, sizeof line, file ) != NULL ){
        interpretator(line);
    }
    fclose(file);
    return 0;
}
