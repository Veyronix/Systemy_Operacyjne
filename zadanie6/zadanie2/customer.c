#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>

#define SERVER_QUEUE_NAME   "/server1"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int kind_of_giving_arguments = 0;
int index_from_server;
char *in_buffer;
char *out_buffer;
char client_queue_name [10];
mqd_t qd_server, qd_client; 

void handlerSIGINT(){
    exit(0);
}

void close_customer_qd(){
    sprintf(out_buffer,"%d %s",index_from_server,"STOP");
    if (mq_send (qd_server, out_buffer, strlen (out_buffer), 0) == -1) {
        perror ("Client: Not able to send message to server");
    }
    printf("\nZAMYKAM SIE\n");
    mq_close(qd_client);
    mq_unlink(client_queue_name);
}
int main (int argc, char **argv)
{
    if(argc == 1){
        kind_of_giving_arguments = 0;
    }
    else if(argc == 3){
        if(strcmp(argv[1],"-file")!=0){
            printf("zly pierwszy argument");
            exit(1);
        }
        kind_of_giving_arguments = 1;
    }
    srand(time(NULL));
    char tmp[10];
    for( int i = 0; i < 10; ++i){
        tmp[i] = 'A' + rand()%26; 
    }
    sprintf (client_queue_name, "/%s", tmp);
    printf("%s\n",client_queue_name);
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Client: mq_open (client)");
        exit (1);
    }

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client: mq_open (server)");
        exit (1);
    }
    
    //registering
    in_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
        out_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
    if (mq_send (qd_server, client_queue_name, strlen (client_queue_name), 0) == -1) {
        perror ("Client: Not able to send message to server");
        exit(2);
    }
    if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
        perror ("Client: mq_receive");
        exit (2);
    }
    atexit(close_customer_qd);
    signal(SIGINT,handlerSIGINT);
    index_from_server = atoi(in_buffer);
    free(in_buffer);
    free(out_buffer);
    char temp_buf [10];
    int i = 1;
    FILE* file;
    if(kind_of_giving_arguments == 1){
        file = fopen(argv[2],"r+");
    }
    while (1) {
        char str[30];
        // send message to server
        in_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
        out_buffer=(char*)calloc(MSG_BUFFER_SIZE,sizeof(char));
        if(kind_of_giving_arguments == 0){
            printf( "Enter a value :");
            gets( str );
        }
        else{
            if(fgets ( str, sizeof str, file ) >0 ){
            }
            else{
                return 0;
            }
        }
        char *operation = strtok(str," ");

        if(strcmp(operation,"MIRROR")==0){
            char* text = strtok(NULL," ");
            sprintf (out_buffer, "%d MIRROR %s",index_from_server,text );
            while((text = strtok(NULL," "))!= NULL){
               sprintf (out_buffer, "%s %s",out_buffer,text ); 
            }
            if (mq_send (qd_server, out_buffer, strlen (out_buffer), 0) == -1) {
                perror ("Client: Not able to send message to server");
                continue;
            }
            
            if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Client: mq_receive");
                exit (1);
            }
            printf("%s\n",in_buffer);
        }
        else if(strcmp(operation,"CALC")==0){
            char *kind_of_operation = strtok(NULL," ");
            if(strcmp(kind_of_operation,"ADD")==0 || 
            strcmp(kind_of_operation,"MUL")==0 || 
            strcmp(kind_of_operation,"SUB")==0 ||
            strcmp(kind_of_operation,"DIV")==0){
                int first_num = atoi(strtok(NULL," "));
                int sec_num = atoi(strtok(NULL," "));
                sprintf (out_buffer, "%d CALC %s %d %d",index_from_server,kind_of_operation,first_num,sec_num);
                if (mq_send (qd_server, out_buffer, strlen (out_buffer), 0) == -1) {
                    perror ("Client: Not able to send message to server");
                    continue;
                }
                if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                    perror ("Client: mq_receive");
                    exit (1);
                }
                printf("%s\n",in_buffer);
            }
            else{
                printf("bad kind of operation");
            }
        }
        else if(strcmp(operation,"TIME")==0){
            sprintf(out_buffer,"%d %s",index_from_server,operation);
            if (mq_send (qd_server, out_buffer, strlen (out_buffer), 0) == -1) {
                perror ("Client: Not able to send message to server");
                continue;
            }
            if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Client: mq_receive");
                exit (1);
            }
            printf("%s\n",in_buffer);
        }
        else if(strcmp(operation,"STOP")==0){
            exit(0);
        }
        else if(strcmp(operation,"END")==0){
            sprintf(out_buffer,"%d %s",index_from_server,operation);
            if (mq_send (qd_server, out_buffer, strlen (out_buffer), 0) == -1) {
                perror ("Client: Not able to send message to server");
                continue;
            }
            exit(0);
        }
        else{
            printf("bad operation\n");
        }
    }

}
