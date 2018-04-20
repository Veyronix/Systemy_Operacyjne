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
int server_queue;
int start_ending = 0; // if start closing que? 0 -> false, 1 -> true

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
    printf("%s\n",homedir);
    key_t code_of_key = ftok(homedir,'l');
    printf("%d\n",code_of_key);
    //server_queue = msgget(IPC_PRIVATE, MSGPERM|IPC_CREAT|IPC_EXCL);
    server_queue = msgget(code_of_key,MSGPERM|IPC_CREAT|IPC_EXCL);
    if(server_queue == -1){
        printf("Cant create server queue");
        return;
    }
    printf("server queue is: %d\n",server_queue);
    int i = 0;
    struct msqid_ds msqid_ds_server_que;
    while(1){
        // IF O TYM CZY JEST COS W KOLEJCE PRZY UZYCIU MSGCTL I MSQID_DS
        msgctl(server_queue,IPC_STAT,&msqid_ds_server_que);
        if(msqid_ds_server_que.msg_qnum != 0){
            if(msgrcv(server_queue,&msgbuf2,MAX_SIZE,0,MSG_NOERROR)>0){
                //printf("ala %s\n",msgbuf2.mtext);
                //printf("typ %d\n",msgbuf2.mtype);
                if(msgbuf2.mtype == 1){ // MIRROR
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = proper_customer_id(tmp);
                    if(customer_id == -1)
                        continue;
                    // int customer_id = atoi(tmp);
                    // if(customer_id == 0){
                    //     if(strcmp(tmp,"0")!=0){
                    //         printf("zly customer id");
                    //         continue;
                    //     }
                    // }
                    // if(customer_id >= index_customers){
                    //     printf("Dont have this customer_id\n");
                    //     continue;
                    // }
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
                    //printf("%s\n",msgbuf2.mtext);
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = atoi(tmp);
                    if(customer_id == 0){
                        if(strcmp(tmp,"0")!=0){
                            printf("zly customer id");
                            continue;
                        }
                    }
                    if(customer_id >= index_customers){
                        printf("Dont have this customer_id\n");
                        continue;
                    }
                    char *kind_of_operation = strtok(NULL," ");
                    int first_num = atoi(strtok(NULL," "));
                    int sec_num = atoi(strtok(NULL," "));
                    if(index_customers < customer_id){
                        printf("customers_id doesnt exist");
                        continue;
                    }
                    char out[20];
                    
                    if(strcmp(kind_of_operation,"ADD")==0){
                        // out = first_num * sec_num;
                        sprintf(msgbuf1.mtext, "%d", first_num + sec_num);
                    } 
                    else if(strcmp(kind_of_operation,"MUL")==0){
                        // out = first_num * sec_num;
                        sprintf(msgbuf1.mtext, "%d", first_num * sec_num);
                    }
                    else if(strcmp(kind_of_operation,"SUB")==0){
                        // out = first_num - sec_num;
                        sprintf(msgbuf1.mtext, "%d", first_num - sec_num);
                    }
                    else if(strcmp(kind_of_operation,"DIV")==0){
                        // out = first_num / sec_num;
                        if(sec_num != 0){
                            sprintf(msgbuf1.mtext, "%.1f", ((float)(first_num) / (float)(sec_num)));
                        }
                        else{
                            sprintf(msgbuf1.mtext, "%s", "DONT DIVIDE BY 0!");
                        }
                    }
                    else{
                        printf("bad ");
                    }
                    msgbuf1.mtype = 2;
                    //printf("wynik dzialania to %s\n",msgbuf1.mtext);
                    if(msgsnd(customers[customer_id], &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                        printf("cant send message to customer\n");
                    }

                }
                else if(msgbuf2.mtype == 3){ // TIME
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = atoi(tmp);
                    if(customer_id == 0){
                        if(strcmp(tmp,"0")!=0){
                            printf("zly customer id");
                            continue;
                        }
                    }
                    if(customer_id >= index_customers){
                        printf("Dont have this customer_id\n");
                        continue;
                    }
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    sprintf(msgbuf1.mtext," %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    msgbuf1.mtype = 3;
                    //printf("godzina to %s\n",msgbuf1.mtext);
                    if(msgsnd(customers[customer_id], &msgbuf1, sizeof(msgbuf1.mtext), 0)<0){
                        printf("cant send message to customer\n");
                    }

                }
                else if(msgbuf2.mtype == 4){ // END
                    char *tmp = strtok(msgbuf2.mtext," ");
                    int customer_id = atoi(tmp);
                    if(customer_id == 0){
                        if(strcmp(tmp,"0")!=0){
                            printf("zly customer id");
                            continue;
                        }
                    }
                    if(customer_id >= index_customers){
                        printf("Dont have this customer_id\n");
                        continue;
                    }
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
            printf("skonczylem sie, chyba \n");
            return;
        }
        i++;
    }


    return 0;
}
