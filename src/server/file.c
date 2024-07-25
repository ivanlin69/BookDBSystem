#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "file.h"

// for new files
int createDBFile(char *filename){
    // make sure that the file does not exist in the first place
    int dbfd = open(filename, O_RDONLY);
    if(dbfd != -1){
        close(dbfd);
        printf("The file already exists.\n");
        return -1;
    }

    // then we create the file safely
    dbfd = open(filename, O_RDWR | O_CREAT, 0644);
    if(dbfd == -1){
        close(dbfd);
        perror("open");
        return -1;
    }
    return dbfd;
}

// for already existing files
int openDBFile(char *filename){
    int dbfd = open(filename, O_RDWR, 0644);
    if(dbfd == -1){
        close(dbfd);
        perror("open");
        return -1;
    }
    return dbfd;
}
