#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <memory.h>
#include <time.h>

void createChildren(int N);
pid_t *listOfChildren;
int actualChild = 0;
int amountOfRequests = 0;
int amountOfSuicide = 0;
int killed = 0;
int K;
int N;
pid_t parentPID;
int main(int argc, char*argv[]){
    if(argc != 3){
        printf("Bad amount of arguments\n");
        exit(1);
    }
    N = atoi(argv[1]);
    K = atoi(argv[2]);
    if(N == 0 || K == 0 || N < K){
        printf("Bad arguments of numbers\n");
        exit(1);
    }
    listOfChildren = malloc(N * sizeof(pid_t));
    
    createChildren(N);
    return 0;
}
void handlerSIGUSR1(int sig, siginfo_t *siginfo, void *context){
    printf("<%d> received request from <%d>\n", getpid(), siginfo->si_pid);
    if(amountOfRequests<K){

        listOfChildren[actualChild] = siginfo->si_pid;
        actualChild++;
    }
    else if(amountOfRequests == K){
        for(int i = 0; i<K;i++){
            kill(listOfChildren[i],SIGUSR2);
        }
        kill(siginfo->si_pid,SIGUSR2);
        killed += K+1;
    }
    else{
        kill(siginfo->si_pid,SIGUSR2);
        killed++;
    }
    amountOfRequests++;
}
void handlerSIGUSR2(){

}
void handler_sigall(int sig_num, siginfo_t *sig_info, void *sig_context){
    printf("<%d> ended child <%d> ended with %d\n", getpid(), sig_info->si_pid,\
            sig_info->si_value.sival_int);
}
void handlerSIGINTChild(){
    if(getppid() == parentPID)
        raise(SIGKILL);
}
void handlerSIGINT(){
    for(int i = 0;i<actualChild;i++){
        kill(SIGINT,listOfChildren[i]);

    }
    printf("\nZakonczenie programu \n");
    exit(0);
}
void set_handlers(){
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.sa_sigaction = handlerSIGUSR1;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_sigaction = handler_sigall;
    for(int i=SIGRTMIN; i<=SIGRTMAX; i++){
      sigaction(i, &act, NULL);
    }
    act.sa_sigaction = handlerSIGINT;
    sigaction(SIGINT,&act,NULL);
}

void createChildren(int N){
    set_handlers();
   
    parentPID = getpid();
    pid_t pid;
    for(int i = 0; i < N;i++){
        pid = fork();

        if(pid == 0) {
            struct sigaction act;
            act.sa_sigaction = handlerSIGUSR2;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            sigaction(SIGUSR2, &act, NULL);
            act.sa_sigaction = handlerSIGINTChild;
            sigaction(SIGINT,&act,NULL);
            srand(time(NULL));
            int timeOfSleep = rand()%10 + 1;
            //printf("%d\n",timeOfSleep);
            sleep(timeOfSleep);
            //printf("procesu %d czas spania to %d\n",getppid(),timeOfSleep);
            kill(getppid(), SIGUSR1);
            
            listOfChildren[actualChild] = pid;
            actualChild++;
            amountOfSuicide++;
            pause();
            union sigval a;
            a.sival_int = timeOfSleep;
            sigqueue(getppid(), rand() % (SIGRTMAX - SIGRTMIN) + SIGRTMIN, a);
            
            exit(timeOfSleep);
        }
        else{
            sleep(2);
        }
    }
    while(1){
        if(killed == N)break;

    }
    sleep(2);
}
