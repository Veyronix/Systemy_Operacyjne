#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

pid_t CHILD_PID = 0;
int RUN_DATE = 1;
int STOPPED = 0;

void handlerSIGTSTP(int signum){

    if(STOPPED == 0) {
	printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
        STOPPED = 1;
        kill(CHILD_PID, 2);
        signal(SIGTSTP, handlerSIGTSTP);
    }
    else{
        STOPPED = 0;
        RUN_DATE = 1;
        signal(SIGTSTP, handlerSIGTSTP);
    }
}

void handlerSIGINT(int signum){
	printf("Odebrano sygnał SIGINT");
	kill(CHILD_PID, 2);
	exit(0);
}
int main(){
    signal(SIGTSTP, handlerSIGTSTP);
	signal(SIGINT,handlerSIGINT);
    int started = 1;

    while (CHILD_PID != 0 || started == 1){
        started = 0;
        if (RUN_DATE){
            CHILD_PID = fork();
            RUN_DATE = 0;
        }
    }
    execl("./data.sh", "data", NULL);
    return 0;
}

