#include "myheader.h"  /* Include the header (not strictly necessary here) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>

#define PROC_DIRECTORY "/proc"
#define STATUS_FILE "status"



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