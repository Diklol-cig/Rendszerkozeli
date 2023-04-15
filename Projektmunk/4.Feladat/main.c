#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "myheader.h"

int main() {
    int pid = FindPID();
    printf("PID: %d\n", pid);
    return 0;
}
