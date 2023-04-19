#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "myheader.h"


int main(int argc, char* argv[]) {
    if (namecheck(argc,argv) == 1){
        exit(1);
    } //Megnézi hogy a futtatható állomány neve chart-e és a parancssori argumentumokat is ellenőrzi
    int modes [4] = modecheck(argc,argv); //Megnézi hogy a parancssori argumentumok jók-e
    srand(time(NULL));
    int *values=NULL;    
    int NumValues=Measurement(&values);
    //printf("The length of the array is: %d\n",NumValues);
    BMPcreator(values, NumValues);
    //SendViaFile(values, NumValues);
    
    int PID = FindPID();

    printf("The PID of the process is: %d\n",PID);
}