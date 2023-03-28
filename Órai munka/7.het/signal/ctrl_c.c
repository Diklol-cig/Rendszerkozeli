/*** Example of catching signal ***/
#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void ToDo(int sig){
  printf("\n  Ouch! It hurts.\n");
  signal(SIGINT, SIG_DFL); // back to default signal handling
//  signal(SIGINT, SIG_IGN); // ignoring signal
  }

int main(){
  int i;

  signal(SIGINT, ToDo);

  printf("Start...\n");
  for(i=1;i<=10;i++){
    sleep(1);
    printf(" sleeptime: %ds\n",i);
    }
  return 0;
  }
