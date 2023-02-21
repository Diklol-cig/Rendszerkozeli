#include <stdio.h>
#include <string.h>

int checkname(char *name){
    // Ellenőrizzük, hogy a program neve "chart" legyen
    if (strcmp(name, "./chart") != 0) {
        printf("Hiba: A futtatható állomány neve 'chart' kell legyen!\n");
        return 1;
    }    
}

int checkdup(int argc, char *argv[]){
    int dup = 0;
    for (int i = 1; i < argc; i++) {
        for(int j = 1; j < argc; j++) {
            {
                if (strcmp(argv[i], argv[j]) == 0) {
                    dup++;
                }
            }
        }
    }

    if(dup>0){
        return 1;//Van duplikalt
    }
    return 0;
}



int main(int argc, char* argv[]) {
    checkname(argv[0]);

    // Ellenőrizzük a parancssori argumentumokat
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0) {
            // Kiírjuk a program verziószámát, elkészültének dátumát és a fejlesztő nevét
            printf("Chart program verzió 1.0\n");
            printf("Elkészült: 2023. február 15.\n");
            printf("Fejlesztő: Dáni L\n");
            return 0;
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

    printf("%d",send_mode);

    printf ("Program mode: ");
    printf(send_mode == 1 ? "Send Mode\n" : "Receive Mode\n");
    printf("Kommunkiacios mode: ");
    printf(file_mode == 1 ? "File Mode\n" : "Socket Mode\n");
}
