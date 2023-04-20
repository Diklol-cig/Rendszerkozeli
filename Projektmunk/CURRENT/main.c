#include "RKPheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<omp.h>


int main(int argc, char* argv[]) {

  signal(SIGINT, signal_handeler); //SIGINT signal kezelése
    if (namecheck(argc,argv) == 1){
        exit(1);
    } //Megnézi hogy a futtatható állomány neve chart-e és a parancssori argumentumokat is ellenőrzi
    int modes [4] = {1,0,1,0}; 
    modecheck(modes, argc, argv); //Megnézi hogy a parancssori argumentumok jók-e
    srand(time(NULL));
    execute_commands(modes);


  /*   //printf("The length of the array is: %d\n",NumValues);
    BMPcreator(values, NumValues);
    //SendViaFile(values, NumValues); */
}