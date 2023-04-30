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
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <omp.h>

#define PROC_DIRECTORY "/proc"
#define STATUS_FILE "status"
#define PORT_NO 3333 // The port on which the server is listening

void BMPcreator(int *Values, int NumValues)
{
    int padding = 0;
    int image_width = NumValues;
    int file_size_bytes;
    int center_line;

    if (image_width % 32 == 0)
    {

        file_size_bytes = (image_width * image_width) / 8 + 62;
    }
    else
    {
        file_size_bytes = ((image_width + (32 - (image_width % 32))) * (image_width)) / 8 + 62;
        padding = 32 - (image_width % 32);
    }
    unsigned char *bitmap = (unsigned char *)calloc(file_size_bytes, 1 * sizeof(char));

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
    for (int i = 30; i < 34; i++)
        bitmap[i] = 0;
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
    center_line = image_width / 2 + 1;

    for (int i = 0; i < NumValues; i++)
    {

        char mask = (char)int_pow(2, 7 - (i % 8)); //The mask variable determines which bit to set during the current iteration. Using the int_pow function, mask takes on the powers of 2 from 128 down to 1 based on the value of 7 minus i modulo 8.

        int curr_line_offset = center_line + Values[i];  //The curr_line_offset variable determines the vertical line on the image that should be shifted by the Values[i] value. This is necessary because the bitmap has no center point.

        int offset_index = ((image_width + padding) / 8) * curr_line_offset; //The offset_index variable determines the horizontal row within the image to which the current byte needs to be added. The padding variable represents the pad added to the data for each row of bytes in the bitmap.

        int curr_byte_index = offset_index + (i / 8);  //The curr_byte_index variable determines which byte to set the bit in during the current iteration. Using offset_index and curr_byte_index, the current bit's location in the bitmap can be determined.

        bitmap[62 + curr_byte_index] |= mask;  //bitmap[62 + curr_byte_index] represents a specific byte within the bitmap to set the bit in. Using the |= operator, the appropriate bit is set. The 62 leaves room for metadata related to the bitmap, which is stored at the beginning of the array.
    }

    int f = open("chart.bmp", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

    write(f, bitmap, file_size_bytes); // File size in bytes
    close(f);
    free(bitmap);
}

void write_help()
{
    // Kiírjuk a program használati útmutatóját
    char errorok[] = "exit(1): Hibás parancssori argumentumok vagy programnév került megadásra.\n"
                     "exit(2): Nem található receive módban futó chart program, ezért a művelet nem hajtható végre.\n"
                     "exit(3): A megadott fájlt nem sikerült megnyitni.\n"
                     "exit(4): Nem sikerült létrehozni a socketet, ami szükséges a kommunikációhoz.\n"
                     "exit(5): Küldési hiba történt a Socketen keresztül.\n"
                     "exit(6): Fogadási hiba történt a Socketen keresztül.\n"
                     "exit(7): A kapott értékek nem egyeznek meg.\n"
                     "exit(8): Nem sikerült bindelni a socketet, ami szükséges a kommunikációhoz.\n"
                     "exit(9): Nem sikerült kapcsolatot létrehozni a szerverrel.\n"
                     "exit(10): A program nem találta a szükséges signált a végrehajtáshoz.\n";

    printf("\nChart program használata:\n");
    printf("chart [--version | --help] [-send | -receive] [-file | -socket]\n");
    printf("\n");
    printf("--version: Kiírja a program verziószámát, elkészültének dátumát és a fejlesztő nevét.\n");
    printf("--help: Kiírja a program használati útmutatóját.\n");
    printf("-send: A program küldőként működik.\n");
    printf("-receive: A program fogadóként működik.\n");
    printf("-file: A kommunikáció fájl segítségével történik.\n");
    printf("-socket: A kommunikáció socketen keresztül történik.\n");
    printf("\n");
    printf("%s", errorok);
}

int namecheck(int argc, char *argv[])
{ // Megnézi hogy a futtatható állomány neve chart-e és a parancssori argumentumokat is ellenőrzi

    // Ellenőrizzük, hogy a program neve "chart" legyen
    if (strcmp(argv[0], "./chart") != 0)
    {
        printf("Hiba: A futtatható állomány neve 'chart' kell legyen!\n");
        exit(1);
    }
    // Ellenőrizzük a parancssori argumentumokat
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if ((strcmp(argv[i], "-send") == 0) || (strcmp(argv[i], "-receive") == 0) || (strcmp(argv[i], "-file") == 0) || (strcmp(argv[i], "-socket") == 0) || (strcmp(argv[i], "--version") == 0) || (strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "--tutorial") == 0))
            {
                continue;
            }
            else
            {
                fprintf(stderr, "Hiba: Hibás parancssori argumentumok vagy programnév került megadásra.\n");
                write_help();
                exit(1);
            }
        }
        if (strcmp(argv[1], "--version") == 0)
        {
            // Kiírjuk a program verziószámát, elkészültének dátumát és a fejlesztő nevét
            #pragma omp parallel sections
            {
            #pragma omp section
                {
                    printf("Chart program verzió 1.0 (Még szép hogy elsőre sikerült) --- Thread(%d)\n", omp_get_thread_num());
                }
            #pragma omp section
                {
                    printf("Elkészült: 2023. február 15. --- Thread(%d)\n", omp_get_thread_num());
                }
            #pragma omp section
                {
                    printf("Fejlesztő: Dáni L --- Thread(%d)\n", omp_get_thread_num());
                }
            }
            exit(0);
        }
        else if (strcmp(argv[1], "--help") == 0 || checkdup(argc, argv))
        {
            write_help();
            exit(0);
        }
        else if (strcmp(argv[1], "--tutorial") == 0)
        {
            // Kiírjuk a program használati útmutatóját
            printf("Chart program használata:\n");
            system("xdg-open https://youtu.be/vqH-VRpqi3E?t=94 &");
            exit(0);
        }
    }
}

int checkdup(int argc, char *argv[])
{
    int dup = 0;
    for (int i = 1; i < argc - 1; i++)
    {
        for (int j = i + 1; j < argc; j++)
        {
            {
                if (strcmp(argv[i], argv[j]) == 0)
                {
                    dup++;
                }
            }
        }
    }
    if (dup >= 1)
    {
        // printf("---%d---\n",dup);
        return 1; // Van duplikalt
    }
    return 0;
}

void signal_handeler(int sig)
{
    if (sig == SIGUSR1)
    {
        ReceiveViaFile(sig);
    }
    else if (sig == SIGINT || sig == SIGTERM)
    {
        printf("\nSikeresen leállítva!\n");
        exit(0);
    }
    else if (sig == SIGALRM)
    {
        printf("Nem sikerült a kapcsolatot létrehozni!\n");
        exit(9);
    }
    else
    {
        fprintf(stderr, "A program nem talált signalt!\n");
        exit(10);
    }
}

void execute_commands(int modes[])
{
    int NumValues;
    srand(time(NULL));
    int *values = NULL;

    if (modes[0] == 1 && modes[2] == 1)
    {
        //printf("Kuldo fajl\n");
        NumValues = Measurement(&values);
        SendViaFile(values, NumValues);
        free(values);
    }
    if (modes[0] == 1 && modes[3] == 1)
    {
        //printf("Kuldo socket\n");
        NumValues = Measurement(&values);
        SendViaSocket(values, NumValues);
    }
    if (modes[1] == 1 && modes[2] == 1)
    {
        //printf("Fogado fajl\n");
        while (1)
        {
            printf("Waiting for signal\n");
            printf("pid_ %d\n", getpid());
            signal(SIGUSR1, signal_handeler);
            pause();
        }
    }
    if (modes[1] == 1 && modes[3] == 1)
    {
        ReceiveViaSocket();
        //printf("Fogado socket\n");
    }
}

void modecheck(int *modes, int argc, char *argv[])
{ // Megnézi hogy a program milyen módban fut

    // Az alapértelmezett üzemmód a küldő üzemmód

    // Az alapértelmezett kommunikációs mód a fájl

    // Ellenőrizzük a további argumentumokat

    int send_flag, receive_flag, file_flag, socket_flag = 0;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-send") == 0)
        {
            modes[0] = 1;
            modes[1] = 0;
            send_flag = 1;
        }
        else if (strcmp(argv[i], "-receive") == 0)
        {
            modes[0] = 0;
            modes[1] = 1;
            receive_flag = 1;
        }
        else if (strcmp(argv[i], "-file") == 0)
        {
            modes[2] = 1;
            modes[3] = 0;
            file_flag = 1;
        }
        else if (strcmp(argv[i], "-socket") == 0)
        {
            modes[2] = 0;
            modes[3] = 1;
            socket_flag = 1;
        }
        else
        {
            // Ha az argumentum érvénytelen, akkor kiírjuk a használati útmutatót
            printf("Hiba: Érvénytelen argumentum! Kérjük, használja a következő opciókat:\n");
            printf("chart [--version | --help] [-send | -recieve] [-file | -socket]\n");
        }
        
        if(send_flag == receive_flag || file_flag == socket_flag){
            write_help();
            exit(1);
        }
    }

    printf("Program mode: ");
    printf(modes[0] == 1 ? "Send Mode\n" : "Receive Mode\n");
    printf("Communication mode: ");
    printf(modes[2] == 1 ? "File Mode\n" : "Socket Mode\n");
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int FindPID()
{
    DIR *d;
    int receiver_pid = -1;
    struct dirent *entry;
    d = opendir("/proc");
    char *dirname = NULL;
    char *filename = NULL;
    while ((entry = readdir(d)) != NULL)
    {
        if ((*entry).d_name[0] > '0' && (*entry).d_name[0] < '9')
        {

            dirname = concat("/proc/", (*entry).d_name);
            filename = concat(dirname, "/status");
            // printf("%s\n",filename );
            char first_line_buffer[256];
            FILE *fp = fopen(filename, "r");
            fscanf(fp, "Name:\t%s\n", first_line_buffer);
            // printf("%s\n\n", first_line_buffer);
            if (strcmp(first_line_buffer, "chart") == 0 && getpid() != atoi((*entry).d_name))
            {
                receiver_pid = atoi((*entry).d_name);
            }
            else
            {
                continue;
            }
            fclose(fp);
            free(dirname);
            free(filename);
        }
    }
    closedir(d);
    free(dirname);
    free(filename);
    return receiver_pid;
}

void write_int(char *p, int value)
{ // Little endianba írja az inteket

    p[0] = value;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

int int_pow(int a, int b)
{ // Hatvanyozas
    int result = 1;
    while (b > 0)
    {
        if (b & 1)
        {
            result *= a;
        }
        a *= a;
        b >>= 1;
    }
    return result;
}

int calculate_size(int min, int sec)
{ // Kiszamolja a krealt tomb meretet
    if (min >= 0 && min < 15)
    {
        if (60 * (min) + sec < 100)
        {
            return 100;
        }
        else
        {
            return 60 * (min) + sec;
        }
    }
    if (min >= 15 && min < 30)
    {
        if (60 * (min - 15) + sec < 100)
        {
            return 100;
        }
        else
        {
            return 60 * (min - 15) + sec;
        }
    }
    if (min >= 30 && min < 45)
    {
        if (60 * (min - 30) + sec < 100)
        {
            return 100;
        }
        else
        {
            return 60 * (min - 30) + sec;
        }
    }
    if (min >= 45 && min <= 59)
    {
        if (60 * (min - 45) + sec < 100)
        {
            return 100;
        }
        else
        {
            return 60 * (min - 45) + sec;
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
    int size_of_array = calculate_size(T4->tm_min, T4->tm_sec);
    // printf("%d\n", size_of_array);
    *p_values = malloc(size_of_array * sizeof(int));
    (*p_values)[0] = 0;
    for (int i = 1; i < size_of_array; i++)
    {
        rand_num = (double)rand() / ((unsigned)RAND_MAX + 1); // 0.0 <= y < 1.0

        if (rand_num <= 0.48571)
        {
            (*p_values)[i] = (*p_values)[i - 1] + 1;
        }
        else if (rand_num > 0.48571 && rand_num < 0.8348)
        {
            (*p_values)[i] = (*p_values)[i - 1] - 1;
        }
        else
        {
            (*p_values)[i] = (*p_values)[i - 1];
        }
    }
    return size_of_array;
}

void SendViaFile(int *Values, int NumValues)
{
    pid_t pid = FindPID();
    printf("FINDPID: %d\n SAJAT: %d\n", pid, getpid());
    if (pid == -1)
    {
        fprintf(stderr, "Nincsen receive modeban chart program!\n");
        exit(2);
    }

    char *fileName = concat(getenv("HOME"), "/Measurement.txt");
    FILE *file = fopen(fileName, "w");

    // megnyitjuk a fájlt írásra, ha nem sikerül, akkor hibát jelezünk
    if (file == NULL)
    {
        printf("Hiba: nem sikerült megnyitni a fájlt.\n");
        exit(3);
    }

    // végigmegyünk a tömb elemein és soronként kiírjuk a fájlba
    for (int i = 0; i < NumValues; i++)
    {
        fprintf(file, "%d\n", Values[i]);
    }

    kill(pid, SIGUSR1);
    // lezárjuk a fájlt
    fclose(file);

    printf("Az adatok sikeresen el lettek küldve a Measurement.txt fájlba.\n");
}

void ReceiveViaFile(int sig)
{
    char *fileName = concat(getenv("HOME"), "/Measurement.txt");
    FILE *file = fopen(fileName, "r");

    int *data = NULL;

    int tmp;
    int count = 0;

    while (fscanf(file, "%d", &tmp) != EOF)
    {
        count++;
        data = realloc(data, count * sizeof(int));
        data[count - 1] = tmp;
    }

    for (int i = 0; i < count; i++)
    {
        printf("%d, ", data[i]);
    }
    puts("");

    BMPcreator(data, count);

    free(data);
}

void SendViaSocket(int *Values, int NumValues)
{                              // Kliens
    int s;                     // socket ID
    int bytes;                 // received/sent bytes
    int flag;                  // transmission flag
    char on;                   // sockopt option
    int received_values;       // received values
    int server_size;           // length of the sockaddr_in server
    struct sockaddr_in server; // address of server

    /************************ Initialization ********************/
    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT_NO);
    server_size = sizeof server;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        fprintf(stderr, "Socket creation error.\n");
        exit(4);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    bytes = sendto(s, &NumValues, sizeof(int), flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "Sending error.\n");
        exit(5);
    }

    signal(SIGALRM, signal_handeler); // Ha nem lát semmit a kliens, akkor leáll
    alarm(1);

    bytes = recvfrom(s, &received_values, sizeof(int), flag, (struct sockaddr *)&server, &server_size);
    if (bytes < 0)
    {
        fprintf(stderr, "Receiving error.\n");
        exit(6);
    }

    if (NumValues != received_values)
    {
        fprintf(stderr, "Nem egyenloek");
        exit(7);
    }

    printf("\nA kapcsolat rendben!\n");

    bytes = sendto(s, Values, sizeof(int) * NumValues, flag, (struct sockaddr *)&server, server_size);
    if (bytes <= 0)
    {
        fprintf(stderr, "Sending error.\n");
        exit(5);
    }
}

void ReceiveViaSocket()
{ // Szerver

    int s;
    int *Values = NULL;
    int NumValues;

    /************************ Declarations **********************/
    int bytes;                 // received/sent bytes
    int err;                   // error code
    int flag;                  // transmission flag
    char on;                   // sockopt option
    int received_values;       // received values
    unsigned int server_size;  // length of the sockaddr_in server
    unsigned int client_size;  // length of the sockaddr_in client
    struct sockaddr_in server; // address of server
    struct sockaddr_in client; // address of client

    /************************ Initialization ********************/
    on = 1;
    flag = 0;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NO);
    server_size = sizeof(server);
    client_size = sizeof(client);

    /************************ Creating socket *******************/
    s = socket(AF_INET, SOCK_DGRAM, 0);

    if (s < 0)
    {
        fprintf(stderr, "Socket creation error.\n");
        exit(4);
    }
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof on);

    err = bind(s, (struct sockaddr *)&server, server_size);
    if (err < 0)
    {
        fprintf(stderr, "Binding error.\n");
        exit(8);
    }

    while (1)
    {
        printf("Kapcsolódásra várakozás..\n");
        bytes = recvfrom(s, &NumValues, sizeof(int), flag, (struct sockaddr *)&client, &client_size);
        if (bytes < 0)
        {
            fprintf(stderr, "Receiving error.\n");
            exit(6);
        }
        printf("The array size is %d\n", NumValues);

        Values = (int *)malloc(NumValues * sizeof(int));

        bytes = sendto(s, &NumValues, sizeof(int), flag, (struct sockaddr *)&client, client_size);

        if (bytes <= 0)
        {
            fprintf(stderr, "Sending error.\n");
            exit(5);
        }

        bytes = recvfrom(s, Values, sizeof(int) * NumValues, flag, (struct sockaddr *)&client, &client_size);
        for (int i = 0; i < NumValues; i++)
        {
            printf("%d, ", Values[i]);
        }
        BMPcreator(Values, NumValues);
        puts("");
    }
}
