#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>
#include<signal.h>

#define SERVER_QUEUE_NAME   "/server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 20
#define MAX_CUSTOMERS 100

mqd_t qd_server, qd_client;

struct customer_qd{
    char name_q[20];
    mqd_t qd_client;
    int opened;
};

char *in_buffer;
char *out_buffer;

struct customer_qd customers[MAX_CUSTOMERS];
int customer_index = 0;

void handlerSIGINT(){
    exit(0);
}

void register_customer(){
    for(int i = 0;i<customer_index;i++){
        if(strcmp(customers[i].name_q,in_buffer)==0){
            printf("Customer already in table\n");
            return;
        }
    }
    printf("Registering %s\n",in_buffer);
    if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
        return;
    }
    customers[customer_index].opened = 1;
    sprintf(customers[customer_index].name_q,"%s",in_buffer);
    customers[customer_index].qd_client = qd_client;
    sprintf(out_buffer,"%d",customer_index);
    if (mq_send (qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
        perror ("Server: Not able to send message to client");
    }
    customer_index++;
}

void delete_all_qd(){
    for(int i = 0;i < customer_index;i++){
        if(customers[i].opened == 1){
            mq_close(customers[i].qd_client);
        }
    }
    printf("Deleting all queues o\n");
    mq_close(qd_server);
    mq_unlink(SERVER_QUEUE_NAME);
}

void choose_operation(){
    char *tmp = strtok(in_buffer," ");
    int first = atoi(tmp);
    if(first == 0){
        if(strcmp(tmp,"0")!=0){
            register_customer();
            return;
        }
    }
    if(first>=customer_index || (customers[first].opened == 0)){
        printf("dont have this customer or closed customer\n");
        return;
    }
    char *operation = strtok(NULL," ");
    if(strcmp(operation,"MIRROR")==0){
        char *message = strtok(NULL," ");
        sprintf(out_buffer,"%s",message);
        while((message = strtok(NULL," "))!= NULL){
            sprintf (out_buffer, "%s %s",out_buffer,message); 
        }
        if (mq_send (customers[first].qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
            perror ("Server: Not able to send message to client");
        }
    }
    else if(strcmp(operation,"TIME")==0){
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(out_buffer," %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        if (mq_send (customers[first].qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
            perror ("Server: Not able to send message to client");
        }
    }
    else if(strcmp(operation,"CALC")==0){
        char *kind_of_operation = strtok(NULL," ");
        int first_num = atoi(strtok(NULL," "));
        int sec_num = atoi(strtok(NULL," "));
        if(strcmp(kind_of_operation,"ADD")==0){
            sprintf(out_buffer, "%d", first_num + sec_num);
        } 
        else if(strcmp(kind_of_operation,"MUL")==0){
            sprintf(out_buffer, "%d", first_num * sec_num);
        }
        else if(strcmp(kind_of_operation,"SUB")==0){
            sprintf(out_buffer, "%d", first_num - sec_num);
        }
        else if(strcmp(kind_of_operation,"DIV")==0){
            if(sec_num != 0){
                sprintf(out_buffer, "%.1f", ((float)(first_num) / (float)(sec_num)));
            }
            else{
                sprintf(out_buffer, "%s", "DONT DIVIDE BY 0!");
            }
        }
        else{
            printf("bad ");
        }
        if (mq_send (customers[first].qd_client, out_buffer, strlen (out_buffer), 0) == -1) {
            perror ("Server: Not able to send message to client");
        }
    }
    else if(strcmp(operation,"END")==0){
            exit(0);
    }
    else if(strcmp(operation,"STOP")==0){
        if(customers[first].opened == 1){
            mq_close(customers[first].qd_client);
            customers[first].opened = 0;
            printf("Closed %s\n",customers[first].name_q);
        }
        
    }
    else{
        printf("dont know operation");
    }
}
int main (int argc, char **argv)
{
    printf ("Server: Hello, World!\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    atexit(delete_all_qd);
    signal(SIGINT,handlerSIGINT);
    while (1) {
        in_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
        out_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
        if (mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror ("Server: mq_receive");
            exit (1);
        }
        choose_operation();
        free(in_buffer);
        free(out_buffer); 
    }
}