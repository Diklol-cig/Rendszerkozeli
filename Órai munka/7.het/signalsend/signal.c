/***  Example of signal sending and receiving  ***/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>

void SignalHandler(int sig){
  if(sig==SIGUSR1)
    printf(" I have received a SIGUSR1 signal.\n");
  if(sig==SIGUSR2)
    printf(" I have received a SIGUSR2 signal.\n");
 }

int main(){
  if(fork()==0){  // child
    srand(getpid());
    sleep(rand()%10);
    if(rand()%2==0)
      kill(getppid(),SIGUSR1);
    else
      kill(getppid(),SIGUSR2);
    }
  else {  // parent
    signal(SIGUSR1,SignalHandler);
    signal(SIGUSR2,SignalHandler);
    pause();
    }
  return 0;
  }