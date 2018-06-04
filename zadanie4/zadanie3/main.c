#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


int CHILD_CNT = 0;
int PARENT_CNT = 0;

void set_handlers_parent();
void set_handlers_child();
void handler_sigusr_child(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigusr2_child(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_sigusr_parent(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_SIGRTMIN_parent(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_SIGRTMIN_child(int sig_num, siginfo_t *sig_info, void *sig_context);
void handler_SIGRTMIN1_child(int sig_num, siginfo_t *sig_info, void *sig_context);
int create_processes(int L, int type);
void send_signal(pid_t ,int type);


int main(int argc, char *argv[])
{
    int L , type;
    if (argc < 3)
        exit(2);
    if ((L = atoi(argv[1])) == 0)
        exit(2);
    if ((type = atoi(argv[2])) == 0 || type < 1 || type > 3)
        exit(2);
    create_processes(L, type);

    return 0;
}


int create_processes(int L, int type){
    pid_t pid = fork();
    if(pid == 0){
        set_handlers_child();
        for(int i = 0;i<L;i++){
            pause();
        }
        printf("Recived %d signals\n", CHILD_CNT);
        pause();
    } else {
        set_handlers_parent();
        sleep(2);
        for(int i = 0;i<L;i++){
            send_signal(pid, type);
            //sleep(0.5);
        }
        if(type == 3){
            kill(pid,SIGRTMIN+1);
        }
        //sleep(1);
        else kill(pid, SIGUSR2);
    }
    printf("Recived %d signals at parent\n", PARENT_CNT);
    return 0;
}

void set_handlers_parent()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = &handler_sigusr_parent;
    sigaction(SIGUSR1, &act, NULL);
    act.sa_sigaction = &handler_SIGRTMIN_parent;
    sigaction(SIGRTMIN, &act, NULL);
}

void set_handlers_child()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;

    act.sa_sigaction = &handler_sigusr_child;
    sigaction(SIGUSR1, &act, NULL);
    act.sa_sigaction = &handler_sigusr2_child;
    sigaction(SIGUSR2, &act, NULL);

    act.sa_sigaction = &handler_SIGRTMIN_child;
    sigaction(SIGRTMIN, &act, NULL);
    act.sa_sigaction = &handler_SIGRTMIN1_child;
    sigaction(SIGRTMIN+1, &act, NULL);
}
void handler_SIGRTMIN_parent(int sig_num, siginfo_t *sig_info, void *sig_context){
    PARENT_CNT++;
}
void handler_sigusr_parent(int sig_num, siginfo_t *sig_info, void *sig_context)
{
    PARENT_CNT++;
}
void handler_SIGRTMIN_child(int sig_num, siginfo_t *sig_info, void *sig_context){
    printf("i 've got a signal!\n");
    CHILD_CNT++;
    printf("child cnt : %d \n", CHILD_CNT);
    kill(getppid(), SIGRTMIN);
}
void handler_sigusr_child(int sig_num, siginfo_t *sig_info, void *sig_context)
{
    printf("i 've got a signal!\n");
    CHILD_CNT++;
    printf("child cnt : %d \n", CHILD_CNT);
    kill(getppid(), SIGUSR1);
}
void handler_sigusr2_child(int sig_num, siginfo_t *sig_info, void *sig_context)
{
    printf("Process ended\n");
    exit(0);
}
void handler_SIGRTMIN1_child(int sig_num, siginfo_t *sig_info, void *sig_context){
    printf("Process ended\n");
    exit(0);
}
void send_signal(pid_t pid ,int type){
    switch(type){
        case 1:
            kill(pid, SIGUSR1);
            break;
        case 2:
            kill(pid, SIGUSR1);
            pause();
            break;
        case 3:
            kill(pid,SIGRTMIN);
            //pause();
            break;
    }
}
