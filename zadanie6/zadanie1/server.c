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
#include<signal.h>

#define MSGPERM 0600


int MAX_SIZE = 100;
struct msgbuf {
    long mtype;         /* typ komunikatu   */
    char mtext[100];      /* tresc komunikatu */
}msgbuf1,msgbuf2;


int customers[100];
int index_customers = 0;
int server_queue;
int start_ending = 0; // if start closing que? 0 -> false, 1 -> true

void handlerSIGINT(){
    exit(0);
}

void delete_server_que(){
    msgctl(server_queue,IPC_RMID,NULL);
}

void registering_customer(int customer_que){
    for(int i =0;i<index_customers;i++){
        if(customers[i] == customer_que){
            printf("we have already customer with this code");
            return;
        }
    }
    printf("Registering %d\n",customer_que);
    customers[index_customers] = customer_que;
    msgbuf1.mtype = 5;
    msgget(customer_que,MSGPERM);
    sprintf (msgbuf1.mtext, "%d",index_customers );
    if(msgsnd(customer_que, &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
        printf("cant send message to customer\n");
    }
    index_customers++;
    return;
}
int proper_customer_id(char* tmp){
    int customer_id = atoi(tmp);
    if(customer_id == 0){
        if(strcmp(tmp,"0")!=0){
            printf("zly customer id");
            return -1;
        }
    }
    if(customer_id >= index_customers){
        printf("Dont have this customer_id\n");
        return -1;
    }
    return customer_id;
}

int main(int argc,char* argv[]) {
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    srand(time(NULL));
    key_t code_of_key = ftok(homedir,('A' + random() % 27));
    server_queue = msgget(code_of_key,MSGPERM|IPC_CREAT|IPC_EXCL);
    if(server_queue == -1){
        printf("Cant create server queue");
        return 1;
    }
    FILE *server_number;
    atexit(delete_server_que);
    if ((server_number = fopen("server_number.txt", "wb+"))==NULL){
        printf("Cant create file");
        exit(1);
    }
    char tmp[8];
    sprintf(tmp,"%d",server_queue);
    fwrite(tmp,sizeof(char),7,server_number);
    fclose(server_number);
    printf("server queue is: %d\n",server_queue);
    struct msqid_ds msqid_ds_server_que;
    signal(SIGINT, handlerSIGINT);
    while(1){
        msgctl(server_queue,IPC_STAT,&msqid_ds_server_que);
        if(msqid_ds_server_que.msg_qnum != 0){
            if(msgrcv(server_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
                if(msgbuf2.mtype == 1){ // MIRROR
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = proper_customer_id(tmp);
                    if(customer_id == -1)
                        continue;
                    char *text = strtok(NULL," ");
                    msgbuf1.mtype = 1;
                    sprintf (msgbuf1.mtext, "%s",text );
                    while((text = strtok(NULL," "))!= NULL){
                        sprintf (msgbuf1.mtext, "%s %s",msgbuf1.mtext,text ); 
                    }
                    if(msgsnd(customers[customer_id], &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                        printf("cant send message to customer\n");
                    }
                }
                else if(msgbuf2.mtype == 2){ // CALC
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = proper_customer_id(tmp);
                    if(customer_id == -1)
                        continue;

                    char *kind_of_operation = strtok(NULL," ");
                    int first_num = atoi(strtok(NULL," "));
                    int sec_num = atoi(strtok(NULL," "));
                    if(index_customers < customer_id){
                        printf("customers_id doesnt exist");
                        continue;
                    }
                    char out[20];
                    
                    if(strcmp(kind_of_operation,"ADD")==0){
                        sprintf(msgbuf1.mtext, "%d", first_num + sec_num);
                    } 
                    else if(strcmp(kind_of_operation,"MUL")==0){
                        sprintf(msgbuf1.mtext, "%d", first_num * sec_num);
                    }
                    else if(strcmp(kind_of_operation,"SUB")==0){
                        sprintf(msgbuf1.mtext, "%d", first_num - sec_num);
                    }
                    else if(strcmp(kind_of_operation,"DIV")==0){
                        if(sec_num != 0){
                            sprintf(msgbuf1.mtext, "%.1f", ((float)(first_num) / (float)(sec_num)));
                        }
                        else{
                            sprintf(msgbuf1.mtext, "%s", "DONT DIVIDE BY 0!");
                        }
                    }
                    else{
                        printf("bad expression");
                    }
                    msgbuf1.mtype = 2;
                    if(msgsnd(customers[customer_id], &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                        printf("cant send message to customer\n");
                    }

                }
                else if(msgbuf2.mtype == 3){ // TIME
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = proper_customer_id(tmp);
                    if(customer_id == -1)
                        continue;

                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    sprintf(msgbuf1.mtext," %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    msgbuf1.mtype = 3;
                    if(msgsnd(customers[customer_id], &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                        printf("cant send message to customer\n");
                    }

                }
                else if(msgbuf2.mtype == 4){ // END
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = proper_customer_id(tmp);
                    if(customer_id == -1)
                        continue;
                    printf("ustawiam start ending na 1\n");
                    start_ending = 1;

                }
                else if(msgbuf2.mtype == 5){ // ADD NEW CUSTOMER
                    char *customers_que = strtok(msgbuf2.mtext," ");
                    registering_customer(atoi(customers_que));
                }
                else{
                    printf("Unknown type");
                }

            }
        }
        else if(start_ending == 1){
            struct msqid_ds *tmp;
            msgctl(server_queue,IPC_RMID,tmp);
            printf("ended server \n");
            return 0;
        }
    }
    return 0;
}
