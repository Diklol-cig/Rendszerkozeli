#include "myheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>

#define PROC_DIRECTORY "/proc"
#define STATUS_FILE "status"

int namecheck(int argc, char* argv[]) {  //Megnézi hogy a futtatható állomány neve chart-e és a parancssori argumentumokat is ellenőrzi
    // Ellenőrizzük, hogy a program neve "chart" legyen
    if (strcmp(argv[0], "./chart") != 0) {
        printf("Hiba: A futtatható állomány neve 'chart' kell legyen!\n");
        return 1;
    }

    // Ellenőrizzük a parancssori argumentumokat
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0) {
            // Kiírjuk a program verziószámát, elkészültének dátumát és a fejlesztő nevét
            printf("Chart program verzió 1.0\n");
            printf("Elkészült: 2023. február 15.\n");
            printf("Fejlesztő: Dáni L\n");
            return 0;
        } else if (strcmp(argv[1], "--help") == 0) {
            // Kiírjuk a program használati útmutatóját
            printf("Chart program használata:\n");
            printf("chart [--version | --help] [-send | -receive] [-file | -socket]\n");
            printf("\n");
            printf("--version: Kiírja a program verziószámát, elkészültének dátumát és a fejlesztő nevét.\n");
            printf("--help: Kiírja a program használati útmutatóját.\n");
            printf("-send: A program küldőként működik.\n");
            printf("-receive: A program fogadóként működik.\n");
            printf("-file: A kommunikáció fájl segítségével történik.\n");
            printf("-socket: A kommunikáció socketen keresztül történik.\n");
            return 0;
        }
    }

    // Az alapértelmezett üzemmód a küldő üzemmód
    int send_mode = 1;
    int receive_mode = 0;

    // Az alapértelmezett kommunikációs mód a fájl
    int file_mode = 1;
    int socket_mode = 0;

    // Ellenőrizzük a további argumentumokat
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-send") == 0) {
            send_mode = 1;
        } else if (strcmp(argv[i], "-receive") == 0) {
            send_mode = 0;
            receive_mode = 1;
        } else if (strcmp(argv[i], "-file") == 0) {
            file_mode = 1;
        } else if (strcmp(argv[i], "-socket") == 0) {
            file_mode = 0;
            socket_mode = 1;
        } else {
            // Ha az argumentum érvénytelen, akkor kiírjuk a használati útmutatót
            printf("Hiba: Érvénytelen argumentum! Kérjük, használja a következő opciókat:\n");
            printf("chart [--version | --help] [-send | -recieve] [-file | -socket]\n");
        }
    }

    //printf("%d",send_mode);

    printf ("Program mode: ");
    printf(send_mode == 1 ? "Send Mode\n" : "Receive Mode\n");
    printf("Kommunkiacios mode: ");
    printf(file_mode == 1 ? "File Mode\n" : "Socket Mode\n");
}

int FindPID() {  //Most megtalálja a bash PID-jét
    DIR *dir;
    struct dirent *dir_entry;
    char status_file_path[256], line[256], name[256], *token;
    int pid = -1;

    dir = opendir(PROC_DIRECTORY);
    if (dir == NULL) {
        perror("opendir failed");
        return -1;
    }

    while ((dir_entry = readdir(dir)) != NULL) {
        if (dir_entry->d_type == DT_DIR && isdigit(dir_entry->d_name[0])) {
            snprintf(status_file_path, sizeof(status_file_path), "%s/%s/%s", PROC_DIRECTORY, dir_entry->d_name, STATUS_FILE);
            FILE *status_file = fopen(status_file_path, "r");
            if (status_file == NULL) {
                perror("fopen failed");
                continue;
            }

            if (fgets(line, sizeof(line), status_file) == NULL) {
                perror("fgets failed");
                fclose(status_file);
                continue;
            }

            sscanf(line, "Name:\t%s", name);
            if (strcmp(name, "bash") == 0) {
                while (fgets(line, sizeof(line), status_file) != NULL) {
                    if (strncmp(line, "Pid:\t", 5) == 0) {
                        token = strtok(line + 5, " \t\n\r\f");
                        pid = atoi(token);
                        break;
                    }
                }
            }

            fclose(status_file);

            if (pid != -1) {
                break;
            }
        }
    }

    closedir(dir);

    return pid;
}

void write_int(int *p, int value){ //Little endianba írja az inteket

    p[0] = value;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

int int_pow(int a, int b) { //Hatvanyozas
    int result = 1;
    while (b > 0) {
        if (b & 1) {
            result *= a;
        }
        a *= a;
        b >>= 1;
    }
    return result;
}

int calculate_size(int min, int sec){ //Kiszamolja a krealt tomb meretet
   if(min>= 0 && min < 15){
       if(60*(min)+sec<100){
              return 100;
         }
         else{
              return 60*(min)+sec;
       }
   }
   if(min>= 15 && min < 30){
       if(60*(min-15)+sec<100){
              return 100;
         }
         else{
              return 60*(min-15)+sec;
       }
    }
    if(min>= 30 && min < 45){
        if(60*(min-30)+sec<100){
            return 100;
        }
        else{
            return 60*(min-30)+sec;
        }
    }
    if(min>= 45 && min <=59){
       if(60*(min-45)+sec<100){
              return 100;
         }
         else{
              return 60*(min-45)+sec;
       }
    }
}

int Measurement(int **p_values)
{
    double rand_num;
    struct tm *T4;
    int T2, T3;
    time_t T1;
    T2 = time(&T1);
    T4 = localtime(&T1);
    int size_of_array = calculate_size(T4->tm_min,T4->tm_sec);
    //printf("%d\n", size_of_array);
    *p_values=malloc(size_of_array*sizeof(int));
    (*p_values)[0]=0;
    for(int i=1;i<size_of_array;i++)
    {
        rand_num= (double)rand()/((unsigned)RAND_MAX+1); // 0.0 <= y < 1.0
        
        if(rand_num<=0.48571)
        {
            (*p_values)[i]=(*p_values)[i-1]+1;
        }
        else if(rand_num>0.48571&&rand_num<0.8348)
        {
            (*p_values)[i]=(*p_values)[i-1]-1;
        }
        else
        {
            (*p_values)[i]=(*p_values)[i-1];
        }
    }
    return size_of_array;
}
