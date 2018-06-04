#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/wait.h>

#define WRITE_END 1
#define READ_END 0


int how_much;
int no_pipes;
int **fds;

void subprogram(char *program){
    char *tmp = program;
    int i = 1;
    
    char *command= strtok(tmp, " ");
    if(command==NULL){
        return;
    }
    if(strstr(command, "\n")){
        return;
    }
    char *tmpAll[5];
    tmpAll[0] = command;
    while((tmpAll[i] = strtok(NULL," "))!= NULL){
        i++;
    }
   if (execv(command, tmpAll) == -1) {
            if (execvp(command, tmpAll) == -1) {
                printf("Couldnt find program.");
                exit(1);
            }
        }
    exit(0);
}

int count_characters(const char *str, char character)
{
    const char *p = str;
    int count = 0;

    do {
        if (*p == character)
            count++;
    } while (*(p++));

    return count;
}

void interpretator(char* text){
    if(text[strlen(text)-1] == '\n') {
        text[strlen(text) - 1] = 0;
    }
    char *tmp= malloc(strlen(text)+1);
    strcpy(tmp, text);
    no_pipes = how_much - 1;
    char *order = strtok(tmp,"|");
    if(strstr(order,"\n"))
        return;
    fds = malloc(sizeof(int *) * no_pipes);
    for (int i = 0; i < no_pipes; i++)
    {
        fds[i] = malloc(sizeof(int) * 2);
        pipe(fds[i]);
    }

    pid_t child_pid = fork();
    // doing first subprogram
    if (child_pid == 0)
    {
        if (no_pipes > 0)
        {
            dup2(fds[0][WRITE_END], STDOUT_FILENO);
            for (int i = 0; i < no_pipes; i++)
            {
                close(fds[i][READ_END]);
                close(fds[i][WRITE_END]);
            }
        }
        subprogram(order);
        
    }
    order =strtok(NULL,"|");
    // doing programs after Pipe
    for (int i = 0; i < no_pipes; i++)
    {   
        
        child_pid = fork();
        if (child_pid == 0)
        {
            dup2(fds[i][READ_END], STDIN_FILENO);
            if (i + 1 != no_pipes)
                dup2(fds[i + 1][WRITE_END], STDOUT_FILENO);
            for (int i = 0; i < no_pipes; i++)
            {
                close(fds[i][READ_END]);
                close(fds[i][WRITE_END]);
            }
            subprogram(order);
        }
        order = strtok(NULL,"|");
    }
    for (int i = 0; i < no_pipes; i++)
    {
        close(fds[i][READ_END]);
        close(fds[i][WRITE_END]);
        free(fds[i]);
    }
    free(fds);
    int status = 0;
    waitpid(child_pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 7)
        exit(2);
    if (WIFEXITED(status))
    {
        printf("OK \n");
        return 0;
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
    while ( fgets ( line, sizeof line, file ) >0 ){
        if (line[0] == '\n')
            continue;
        printf("%s",line);
        char *tmp2= malloc(strlen(line)+1);
        strcpy(tmp2, line);
        char *tmp = strtok(tmp2, "|");
        how_much = 0;
        if(tmp != NULL)how_much++;
        while((strtok(NULL,"|"))!= NULL){
            how_much++;
        }      
        interpretator(line);
    }
    fclose(file);
    
    return 0;
}
