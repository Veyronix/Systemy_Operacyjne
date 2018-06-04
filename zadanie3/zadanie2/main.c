#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/wait.h>


void interpretator(char* text){
    if(text[strlen(text)-1] == '\n') {
        text[strlen(text) - 1] = 0;
    }
    char *tmpAll[5];
    char *tmp= malloc(strlen(text)+1);
    strcpy(tmp, text);
    char *command= strtok(tmp, " ");
    if(command==NULL){
        return;
    }
    if(strstr(command, "\n")){
        return;
    }
    int i = 0;
    while((tmpAll[i] = strtok(NULL," "))!= NULL){
        i++;
    }
    printf("\nLaunch: %s\n",text);

    struct rusage usage;
    pid_t pid = fork();
    if (pid > 0) {
        int status;

        wait3(&status,0,&usage);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
            printf("\nChild process didnt done their job.\n");
            exit(2);
        }
        
        if(WIFSIGNALED(status)){
            printf("Terminated proces with code %d" , WTERMSIG(status));
            exit(WTERMSIG(status));
        }


    }
    else if( pid == 0) {

        tmpAll[0] = command;
        printf("%s", command);

        if (execv(command, tmpAll) == -1) {
            if (execvp(command, tmpAll) == -1) {
                printf("Couldnt find program.");
                exit(1);
            }
        }
        exit;
    }
}


int main(int argc,char* argv[]) {

    if(argc != 2){
        printf("Bad amount of arguments");
        exit(1);
    }
    FILE *file = fopen(argv[1],"r+");
    if(file == NULL){
        printf("Cant open file");
        exit(1);
    }
    
    char line [ 128 ];
    while ( fgets ( line, sizeof line, file ) != NULL ){
        interpretator(line);

    }
    fclose(file);
    return 0;
}
