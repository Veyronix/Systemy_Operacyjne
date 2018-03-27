#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <zconf.h>
#include <wait.h>

#include <sys/resource.h>

int poow(int a,int b){
    int sum = 1;
    for(int i = 0;i<b;i++){
        sum*=a;
    }
    return sum;
}
int argumentToInt(int index,char* argv[]){
    int sum = 0;
    for(int j = strlen(argv[index])-1;j>=0;j--){
        if(argv[index][j]<'0' || argv[index][j]>'9'){
            printf("Bad arguments of amount of blocks");
            exit(1);
        }
        sum += (argv[index][j] - '0')*poow(10,(strlen(argv[index])-1)-j);
    }
    return sum;
}

char* modToString(char* fullPath){
    struct stat file;
    char* tab = malloc(11*sizeof(char));
    tab = strcpy(tab,"-----------\0");
    if(!stat(fullPath,&file)){
        if (file.st_mode & S_IRUSR) tab[1] = 'r';
        if (file.st_mode & S_IWUSR) tab[2] = 'w';
        if (file.st_mode & S_IXUSR) tab[3] = 'x';
        if (file.st_mode & S_IRGRP) tab[4] = 'r';
        if (file.st_mode & S_IWGRP) tab[5] = 'w';
        if (file.st_mode & S_IXGRP) tab[6] = 'x';
        if (file.st_mode & S_IROTH) tab[7] = 'r';
        if (file.st_mode & S_IWOTH) tab[8] = 'w';
        if (file.st_mode & S_IXOTH) tab[9] = 'x';
    }
    return tab;
}
int comparatorOfDates(time_t date1,time_t date2, char comparator){
    if(comparator == '<'){ // <
        if(difftime(date1,date2)>86400.000000 ){
            return 1;
        }else return 0;
    }else if(comparator == '='){
        if(difftime(date2,date1)<86400.000000 && difftime(date2,date1)>0){
            return 1;
        }else return 0;
    }else{
        if(difftime(date2,date1)>86400.000000){
            return 1;
        }else return 0;
    }
}
void walkingThroughTreeSys(char *path,time_t date,char comparator){
    DIR* folder = opendir(path);
    struct dirent *dirent1;
    while(folder != NULL && (dirent1 = readdir(folder))!=NULL){
        char *str = malloc(4096*sizeof(char));
        char *brr = malloc(4096*sizeof(char));
        strcpy(str, path);
        strcat(str, "/");
        strcat(str, dirent1->d_name);
        realpath(str, brr);
        free(str);
        str = brr;
        if(dirent1->d_type == DT_DIR && !S_ISLNK(dirent1->d_type)) {
            if (strcmp(dirent1->d_name,".")!=0 && strcmp(dirent1->d_name,"..")!=0) {
                pid_t pid = fork();
                struct rusage usage;
                if(pid == 0){
                    walkingThroughTreeSys(str,date,comparator);
                    exit(0);
                }
                else if(pid >0){
                    int status;
                    wait3(&status,0,&usage);
                    if(WIFEXITED(status) && WEXITSTATUS(status) != 0){
                        printf("\npotomek przegral\n");
                        exit(2);
                    }
                    exit;
                }
                else{
                    printf("\nPid lower than 0.\n");
                    exit(1);
                }

            }
        }
        else if(dirent1->d_type == DT_REG){
            struct stat *buffer = malloc(sizeof(struct stat));
            stat(str,buffer);
            if(comparatorOfDates(date,buffer->st_mtime,comparator)==1) {
                char *mod = modToString(str);
                printf("\n%s\t %zu \t%s\t%s\n", str,buffer->st_size,mod,ctime(&buffer->st_mtime));
            }
            free(buffer);
        }
        free(str);
    }
    closedir(folder);
}

int main(int argc,char* argv[]) {
    printf("dlugosc argc = %d\n", argc);
    if (argc != 6) {
        printf("Bad amount of arguments");
        exit(1);
    }
    if (strcmp("<", argv[2]) == 0 || strcmp(">", argv[2]) == 0 || strcmp("=", argv[2]) == 0) {
        char *path = argv[1];
        int day = argumentToInt(3,argv);
        int month = argumentToInt(4,argv);
        int year = argumentToInt(5,argv);
        struct tm t = { .tm_year=year-1900,.tm_mon = month-1, .tm_mday = day};
        time_t date = mktime(&t);
        if(t.tm_mday != day){
            printf("Not proper date");
            exit(1);
        }
        if(opendir(path) == NULL){
            printf("Bad path");
            exit(1);
        }
        walkingThroughTreeSys(path,date,argv[2][0]);
        return 0;
    }else{
        printf("Bad secound of argument");
        exit(1);
    }
}
