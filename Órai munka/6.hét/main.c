#include <stdio.h>
#include <dirent.h>

int main (int argc, char *argv[]){
    struct dirent *l;
    DIR* d;
    d = opendir("asd");
    l=readdir(d);
    while((l=readdir(d))!=NULL){
        if((*l).d_name[0] != '.'){
            printf("%s\n", l->d_name);
        }
    }
    closedir(d);
}