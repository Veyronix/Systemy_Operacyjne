#include <stdio.h>
#include <stdlib.h>

#include<sys/msg.h>
#include<sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <memory.h>

#include <time.h>
#include <signal.h>
#define MSGPERM 0600


int MAX_SIZE = 100;
struct msgbuf {
    long mtype;         /* typ komunikatu   */
    char mtext[100];      /* tresc komunikatu */
}msgbuf1,msgbuf2;


int customers[100];
int index_customers = 0;
int customer_queue;
int server_queue;
int code_of_customer_from_server;
int kind_of_giving_arguments = 0; // 0 -> z terminala
                                  // 1 -> z pliku
FILE *file;

void handlerSIGINT(){
    exit(0);
}

void delete_customer_que(){
    msgctl(customer_queue,IPC_RMID,NULL);
}

int main(int argc,char* argv[]) {
    if(argc == 1){
        kind_of_giving_arguments = 0;
    }
    else if(argc == 3){
        if(strcmp(argv[1],"-file")!=0){
            printf("bad first argument");
            exit(1);
        }
        kind_of_giving_arguments = 1;
    }
    else{
        printf("Bad amount of arguments");
        exit(1);
    }
    FILE *server_number;
    if ((server_number = fopen("server_number.txt", "r"))==NULL){
        printf("Cant create file");
        exit(1);
    }
    char tmp[8];
    fread(tmp,8,7,server_number);
    server_queue = atoi(tmp);
    if(server_queue == 0){
        if(strcmp(strtok(tmp," "),"0")!=0){
            printf("zla wartosc");
            exit(1);
        }
    }
    fclose(server_number);
    signal(SIGINT, handlerSIGINT);
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    srand(time(NULL));
    key_t code_of_key = ftok(homedir,'A' + (random() % 26));
    while((customer_queue = msgget(code_of_key,MSGPERM|IPC_CREAT|IPC_EXCL))== -1){
        code_of_key = ftok(homedir,'A' + (random() % 26));
    }
    if(customer_queue == -1){
        printf("Cant create customer queue\n");
        exit(1);
    }
    atexit(delete_customer_que);
    printf("customer queue is: %d\n",customer_queue);
    msgbuf1.mtype = 5;
    sprintf (msgbuf1.mtext, "%d",customer_queue );
    long type = 1;
    if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
        printf("cant send message to server\n");
        exit(1);
    }
    if(msgrcv(customer_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
        printf("my code from server is: %s\n",msgbuf2.mtext);
        code_of_customer_from_server = atoi(msgbuf2.mtext);
    }
    if(kind_of_giving_arguments == 1){
        file = fopen(argv[2],"r+");
    }
    int i = 1;
    while(i){
        char str[100];
        if(kind_of_giving_arguments == 0){
            printf( "Enter a value :");
            gets( str );
        }
        else{
            if(fgets ( str, sizeof str, file ) >0 ){}
            else{
                return 0;
            }

        }
        char *operation = strtok(str," ");
        printf("operation: %s\n",operation);

        if(strcmp(operation,"MIRROR")==0){
            char* text = strtok(NULL," ");
            msgbuf1.mtype = 1;
            sprintf (msgbuf1.mtext, "%d %s",code_of_customer_from_server,text );
            while((text = strtok(NULL," "))!= NULL){
               sprintf (msgbuf1.mtext, "%s %s",msgbuf1.mtext,text ); 
            }
            if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                printf("cant send message to server\n");
                continue;
            }
            
            if(msgrcv(customer_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
                printf("Mirror: %s\n",msgbuf2.mtext);
            }
        }
        else if(strcmp(operation,"CALC")==0){
            char *kind_of_operation = strtok(NULL," ");
            if(strcmp(kind_of_operation,"ADD")==0 || 
            strcmp(kind_of_operation,"MUL")==0 || 
            strcmp(kind_of_operation,"SUB")==0 ||
            strcmp(kind_of_operation,"DIV")==0){
                int first_num = atoi(strtok(NULL," "));
                int sec_num = atoi(strtok(NULL," "));
                msgbuf1.mtype = 2;
                sprintf (msgbuf1.mtext, "%d %s %d %d",code_of_customer_from_server,kind_of_operation,first_num,sec_num);
                if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                    printf("cant send message to server\n");
                    continue;
                }
                if(msgrcv(customer_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
                    printf("OUT is: %s\n",msgbuf2.mtext);
                }
            }
            else{
                printf("bad kind of operation");
            }
        }
        else if(strcmp(operation,"TIME")==0){
            msgbuf1.mtype = 3;
            sprintf (msgbuf1.mtext, "%d",code_of_customer_from_server);
            if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                    printf("cant send message to server\n");
                    continue;
            }
            if(msgrcv(customer_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
                printf("Time is: %s\n",msgbuf2.mtext);
            }
        }
        else if(strcmp(operation,"END")==0){
            msgbuf1.mtype = 4;
            printf("sending end\n");
            sprintf (msgbuf1.mtext, "%d",code_of_customer_from_server);
            if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                    printf("cant send message to server\n");
                    continue;
            }
            return 0;
        }
        else if(strcmp(operation,"SUICIDE")==0){
            return 0;
        }
        else{
            printf("bad operation\n");
        }
        
    }

    return 0;
}