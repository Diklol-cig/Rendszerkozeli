#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

int main(int argc,char **argv){
    struct stat inode;
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    
    printf("%s\n",homedir);
    
    struct dirent *l;
    DIR* d;
    char path[1024];
    chdir(homedir);
    do{
        system("clear");
        d = opendir(".");
        l=readdir(d);
            while((l=readdir(d))!=NULL){
                printf("%s\n", l->d_name);
            }
            fgets(path, sizeof(path), stdin);   
            
            if((inode.st_mode&__S_IFDIR)){
                chdir(path);
            }

    }while((inode.st_mode&__S_IFDIR));
    closedir(d);
}