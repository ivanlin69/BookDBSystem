#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "file.h"

// for new files
int createDBFile(char *filename){
    int dbfd = open(filename, O_RDONLY);
    if(dbfd != -1){
        close(dbfd);
        printf("The file already exists.\n");
        return -1;
    }
    dbfd = open(filename, O_RDWR | O_CREAT, 0644);
    if(dbfd == -1){
        close(dbfd);
        perror("open");
        return -1;
    }
    return dbfd;
}

// for existing files
int openDBFile(char *filename){
    int dbfd = open(filename, O_RDWR, 0644);
    if(dbfd == -1){
        close(dbfd);
        perror("open");
        return -1;
    }
    return dbfd;
}
