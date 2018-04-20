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

void delete_customer_que(){
    msgctl(customer_queue,IPC_RMID,NULL);
}
int main(int argc,char* argv[]) {
    if(argc == 2){
        kind_of_giving_arguments = 0;
    }
    else if(argc == 4){
        if(strcmp(argv[2],"-file")!=0){
            printf("zly pierwszy argument");
            exit(1);
        }
        kind_of_giving_arguments = 1;
    }
    server_queue = atoi(strtok(argv[1]," "));
    if(server_queue == 0){
        if(strcmp(strtok(argv[1]," "),"0")!=0){
            printf("zla wartosc");
            exit(1);
        }
    }
    
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    printf("%s\n",homedir);
    srand(time(NULL));
    key_t code_of_key = ftok(homedir,'A' + (random() % 26));
    printf("%d\n",code_of_key);
    //server_queue = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
    customer_queue = msgget(code_of_key,MSGPERM|IPC_CREAT|IPC_EXCL);
    //
    if(customer_queue == -1){
        printf("Cant create customer queue\n");
        return;
    }
    atexit(delete_customer_que);
    printf("customer queue is: %d\n",customer_queue);
    // sign up in server
    msgbuf1.mtype = 5;
    sprintf (msgbuf1.mtext, "%d",customer_queue );
    long type = 1;
    if(msgsnd(server_queue, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
        printf("cant send message to server\n");
        exit(1);
    }
    //sleep(5);
    if(msgrcv(customer_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
        printf("my code from server is: %s\n",msgbuf2.mtext);
        code_of_customer_from_server = atoi(msgbuf2.mtext);
    }
    if(kind_of_giving_arguments == 1){
        file = fopen(argv[3],"r+");
    }
    //
    int i = 1;
    while(i){
        char str[100];
        if(kind_of_giving_arguments == 0){
            printf( "Enter a value :");
            gets( str );
        }
        else{
            if(fgets ( str, sizeof str, file ) >0 ){
            }
            else{
                return;
            }

        }
        char *operation = strtok(str," ");
        printf("operation: %s\n",operation);
        if(strcmp(operation,"MIRROR")==0){
            char* text = strtok(NULL," ");
            msgbuf1.mtype = 1;
            // printf("text to %s\n",text);
            sprintf (msgbuf1.mtext, "%d %s",code_of_customer_from_server,text );
            while((text = strtok(NULL," "))!= NULL){
               sprintf (msgbuf1.mtext, "%s %s",msgbuf1.mtext,text ); 
            }
            printf("caly tekst %s\n",msgbuf1.mtext);
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
                // printf("%d %d\n",first_num,sec_num);
                msgbuf1.mtype = 2;
                sprintf (msgbuf1.mtext, "%d %s %d %d",code_of_customer_from_server,kind_of_operation,first_num,sec_num);
                //printf("wysylam %s\n",msgbuf1.mtext);
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
            return;
        }
        else if(strcmp(operation,"SUICIDE")==0){
            return;
        }
        else{
            printf("bad operation\n");
        }
        
    }

    return 0;
}