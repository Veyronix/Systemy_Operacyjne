#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

const int sizeOfBuffor = 20;
const int sizeOfBlock = 5;

void interpretator(char* text){
    printf("TO JEST TEXT %s\n",text);
    char *tmpAll[5];
    char *tmp= malloc(strlen(text)+1);
    strcpy(tmp, text);
    char *command= strtok(tmp, " ");
    tmpAll[0]=command;
    int i = 1;
    while((tmpAll[i] = strtok(NULL," "))!= NULL){
        i++;
    }
    for(int j =i ;j<strlen(text);j++){
        //tmpAll[j]="";
    }
    int worked = 0;// false
    //Sprawdzic czy jest path itd
    //fork();
    execl("/bin/ls", "ls",NULL);
    if(execl(tmpAll[0], tmpAll[0],tmpAll[1], tmpAll[2], tmpAll[3], tmpAll[4], NULL)!=-1){
        worked=1;
    }
    else {
        char *allPaths = getenv("PATH");
        char *tmpPath = strtok(allPaths, ":");
        //printf("%s",tmpPath);
        char path[1000];
        strcpy(path, tmpPath);
        strcat(path, "/");
        strcat(path, tmpAll[0]);
        if (execvp(path, tmpAll) != -1) {
            worked = 1;
        }
        //printf("%s",path);
        //pid_t pid = fork();
        //while()
        //printf("%s\n",allPaths);
        printf("%s\n",path);
        pid_t pid = fork();
        if (pid > 0) {
            if (execvp(path, tmpAll) != -1) {
                worked = 1;
            } else {
                while ((tmpPath = strtok(NULL, ":")) != NULL) {
                    strcpy(path, tmpPath);
                    strcat(path, "/");
                    strcat(path, tmpAll[0]);
                    printf("%s\n",path);
                    if (execvp(path, tmpAll) != -1) {
                        worked = 1;
                        break;
                    }
                }
            }
        }
    }
}

int main(int argc,char* argv[]) {
    //interpretator(argv[1]);
    //interpretator("ls -a -l");
    if(argc != 2){
        printf("Bad amount of arguments");
        exit(1);
    }
    FILE *file = fopen(argv[1],"r+");
    if(file == NULL){
        printf("Cant open file");
        exit(1);
    }
    int amountsOfBlocks = 1;
    int counterOfBlocks = 1;
    char* buffor = (char*) malloc (sizeOfBuffor + 1);
    while(fread(buffor,sizeOfBlock,amountsOfBlocks,file)){
       // printf(" %s\n",buffor);
        if(feof(file)){
            //printf("%s",buffor);
            interpretator(buffor);
            //wykonaj program dla buffor
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
                //printf("%s\n", function);
                //wykonaj program
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
    //printf(" %s\n",buffor);
    interpretator(buffor);

    return 0;
}
