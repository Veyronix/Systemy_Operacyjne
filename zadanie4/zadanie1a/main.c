#include <stdio.h>
#include <signal.h>
#include <zconf.h>
#include <stdlib.h>

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

int STOPPED = 0;

void handlerSIGINT(int sig_no){
    printf("Odebrano sygnał SIGINT\n");
    //raise(2);
    exit(0);
}
void handlerSIGTSTP(int signum){
    if(STOPPED == 0) {
        STOPPED = 1;
        printf("\n Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu \n");
    }else{
        STOPPED = 0;
    }
}

int main(){
    struct sigaction act;
    act.sa_handler = handlerSIGTSTP;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);

    signal(SIGINT, handlerSIGINT);

    while(1){
        if (!STOPPED)
            printf("%d",system("date"));
	else
		pause();
        sleep(1);
    }
}



