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
#include <signal.h>


#define PROC_DIRECTORY "/proc"
#define STATUS_FILE "status"

void BMPcreator(int *Values, int NumValues){
    

    int padding = 0;
    int image_width = NumValues;
    int file_size_bytes;
    int center_line;
    
    if(image_width%32==0){
        
        file_size_bytes=(image_width*image_width)/8+62;
    }
    else{
        file_size_bytes=((image_width+(32-(image_width%32)))*(image_width))/8+62;
        padding=32-(image_width%32);
    }
    unsigned char* bitmap = (unsigned char*)calloc(file_size_bytes,1*sizeof(char));


    printf("%d\n", image_width);
    
    printf("File size in bytes: %d\n", file_size_bytes);
    // -- FILE HEADER -- //
    // bitmap signature
    bitmap[0] = 'B';
    bitmap[1] = 'M';
    // file size
    write_int(&bitmap[2], file_size_bytes);
    // reserved field (in hex. 00 00 00 00)
    write_int(&bitmap[6], 0);
    // offset of pixel data inside the image
    write_int(&bitmap[10], 62);
    // -- BITMAP HEADER -- //
    // header size
    write_int(&bitmap[14], 40);
    // width of the image
    write_int(&bitmap[18], image_width);
    // height of the image
    write_int(&bitmap[22], image_width);
    // reserved field
    bitmap[26] = 0x01;
    bitmap[27] = 0x00;
    // number of bits per pixel
    bitmap[28] = 0x01;
    bitmap[29] = 0x00;
    // compression method (no compression here)
    for(int i = 30; i < 34; i++) bitmap[i] = 0; 
    // size of pixel data
    bitmap[34] = 0;
    bitmap[35] = 0;
    bitmap[36] = 0;
    bitmap[37] = 0;
//-----------------------------------------TO_DO-----------------------------------------
    // horizontal resolution of the image - pixels per meter (2835)
    write_int(&bitmap[38], 3937);
    // vertical resolution of the image - pixels per meter (2835)
    write_int(&bitmap[42], 3937);
//-----------------------------------------TO_DO-----------------------------------------
    write_int(&bitmap[46], 0);
    write_int(&bitmap[50], 0);
    // color pallette information
    bitmap[54] = 0xCC;
    bitmap[55] = 0x00;
    bitmap[56] = 0xCC;
    bitmap[57] = 0xFF;
    bitmap[58] = 0x00;
    bitmap[59] = 0xFF;
    bitmap[60] = 0x00;
    bitmap[61] = 0xFF;


    int line_bit_count = image_width + padding;
    center_line = image_width/2+1;
    
    for(int i = 0; i<NumValues; i++){

        char mask = (char)int_pow(2, 7 - (i % 8));

        int current_line_offset = center_line + Values[i];

        int offset_line_index = ((image_width + padding)/8) * current_line_offset;

        int current_byte_index = offset_line_index + ( i / 8 );

        bitmap[62 + current_byte_index] |= mask;
    }
    
    int f = open("chart.bmp", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    
    write(f, bitmap, file_size_bytes); //File size in bytes
    close(f);
    free(bitmap);
}

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
            return 1;
        } else if (strcmp(argv[1], "--help") == 0|| checkdup(argc, argv)) {
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
            return 1;
        }
    }
}

int checkdup(int argc, char *argv[]){
    int dup = 0;
    for (int i = 1; i < argc -1; i++) {
        for(int j = i + 1; j < argc; j++) {
            {
                if (strcmp(argv[i], argv[j]) == 0) {
                    dup++;
                }
            }
        }
    }
    if(dup>=1){
        //printf("---%d---\n",dup);
        return 1;//Van duplikalt
    }
    return 0;
}

void signal_handeler(int sig){
  if( sig == SIGUSR1){
    ReceiveViaFile(sig);
  }
  else
  {
    fprintf(stderr,"Error: Signal not found");
    exit(7);
  }
}

void execute_commands(int modes[]){
    srand(time(NULL));
    int *values=NULL;    

    if(modes[0] == 1 && modes[2] == 1){
        //printf("Kuldo fajl\n");
        int NumValues=Measurement(&values);
        SendViaFile(values,NumValues);
    }else if(modes[0] == 1 && modes[3] == 1){
        //printf("Kuldo socket\n");
        //send_socket();
        
    }else if(modes[1] == 1 && modes[2] == 1){
        //printf("Fogado fajl\n");
        while (1)
        {
      printf("Waiting for signal\n");
      signal(SIGUSR1,signal_handeler);
      pause();
        }
    }else if(modes[1] == 1 && modes[3] == 1){
        //printf("Fogado socket\n");
        receive_socket();
    }
}

int modecheck(int argc, char* argv[]) { //Megnézi hogy a program milyen módban fut
    
    int modes[4] = {1,0,1,0};
    // Az alapértelmezett üzemmód a küldő üzemmód

    // Az alapértelmezett kommunikációs mód a fájl

    // Ellenőrizzük a további argumentumokat
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-send") == 0) {
            modes[0] = 1;
            modes[1] = 0;
        } else if (strcmp(argv[i], "-receive") == 0) {
            modes[0] = 0;
            modes[1] = 1;
        } else if (strcmp(argv[i], "-file") == 0) {
            modes[2] = 1;
            modes[3] = 0;
        } else if (strcmp(argv[i], "-socket") == 0) {
            modes[2] = 0;
            modes[3] = 1;
        } else {
            // Ha az argumentum érvénytelen, akkor kiírjuk a használati útmutatót
            printf("Hiba: Érvénytelen argumentum! Kérjük, használja a következő opciókat:\n");
            printf("chart [--version | --help] [-send | -recieve] [-file | -socket]\n");
        }
    }

    //printf("%d",send_mode);

    printf ("Program mode: ");
    printf(modes[0] == 1 ? "Send Mode\n" : "Receive Mode\n");
    printf("Communication mode: ");
    printf(modes[2] == 1 ? "File Mode\n" : "Socket Mode\n");
    
    return modes;
}

int FindPID() {
    DIR *dir;
    struct dirent *dir_entry;
    char status_file_path[256], line[256], name[256], *token;
    int pid = -1, own_pid = getpid();  //getpid() returns the PID of the current program

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

            sscanf(line, "Name:\t%s", name);
            if (strcmp(name, "chart") == 0) {
                while (fgets(line, sizeof(line), status_file) != NULL) {
                    if (strncmp(line, "Pid:\t", 5) == 0) {
                        token = strtok(line + 5, " \t\n\r\f");
                        pid = atoi(token);
                        break;
                    }
                }
            }

            fclose(status_file);

            if (pid != -1 && pid != own_pid) { // Ignore the case where the PID found is the same as the program's own PID
                break;
            }
        }
    }

    closedir(dir);

    if (pid == -1) {
        fprintf(stderr, "Hiba: Nem találtunk vevő üzemmódban futó folyamatot.\n");
        exit(1);
    } else {
        kill(pid, SIGUSR1);
    }

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

void SendViaFile(int *Values, int NumValues) {
    FILE *file;
    char fileName[] = "Measurement.txt";
    int i;

    // megnyitjuk a fájlt írásra, ha nem sikerül, akkor hibát jelezünk
    if ((file = fopen(fileName, "w")) == NULL) {
        printf("Hiba: nem sikerült megnyitni a fájlt.\n");
        return;
    }

    // végigmegyünk a tömb elemein és soronként kiírjuk a fájlba
    for (i = 0; i < NumValues; i++) {
        fprintf(file, "%d\n", Values[i]);
    }

    // lezárjuk a fájlt
    fclose(file);

    printf("Az adatok sikeresen el lettek küldve a Measurement.txt fájlba.\n");
}

void ReceiveViaFile(int sig) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s", getenv("HOME"), "Measurement.txt");
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Could not open file %s\n", filename);
        exit(1);
    }

    float *data = NULL;
    size_t data_size = 0;
    int data_count = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        float value = strtof(line, NULL);
        if (data_count >= data_size) {
            data_size += 10;
            data = realloc(data, data_size * sizeof(float));
            if (data == NULL) {
                fprintf(stderr, "Could not allocate memory\n");
                exit(1);
            }
        }
        data[data_count++] = value;
    }

    fclose(file);

    BMPcreator(values, NumValues);

    free(data);
}