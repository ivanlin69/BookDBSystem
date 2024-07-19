#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parse.h"

int createDBHeader(struct dbHeader **outputHeader){
    struct dbHeader * newHeader = (struct dbHeader *) malloc(sizeof(struct dbHeader));
    if(newHeader == NULL){
        printf("Malloc failed.\n");
        return -1;
    }
    newHeader->magic = HEADER_MAGIC;
    newHeader->version = 1;
    newHeader->count = 0;
    newHeader->filesize = sizeof(struct dbHeader);

    *outputHeader = newHeader;
    return 0;
}

int validateDBHeader(int fd, struct dbHeader **outputHeader){
    if(fd < 0){
        printf("Invalid file descriptor.\n");
        return -1;
    }

    struct dbHeader * header = (struct dbHeader *) malloc(sizeof(struct dbHeader));
    if(header == NULL){
        printf("Malloc failed.\n");
        return -1;
    }
    // read the given header to the buffer, make sure the read size is expected
    if(read(fd, header, sizeof(struct dbHeader)) !=  sizeof(struct dbHeader)){
        perror("read");
        free(header);
        return -1;
    }
    // update the content and align the endianness(from file/network to host)
    header->magic = ntohl(header->magic);
    if(header->magic != HEADER_MAGIC){
        printf("Invalid header magic.\n");
        free(header);
        return -1;
    }
    header->version = ntohs(header->version);
    if(header->version != 1){
        printf("Invalid header version.\n");
        free(header);
        return -1;
    }

    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);

    // use stat to aquire the size of the given file
    struct stat dbStat = {0};
    if(fstat(fd, &dbStat) == -1){
        perror("fstat");
        free(header);
        return -1;
    }
    // make sure the size recorded in the header is the same as its real size
    if(header->filesize != dbStat.st_size){
        printf("Invalid header size.\n");
        free(header);
        return -1;
    }

    *outputHeader = header;
    return 0;
}

int outputDBFile(int fd, struct dbHeader *header){
    if(fd < 0){
        printf("Invalid file descriptor.\n");
        return -1;
    }

    struct dbHeader * dbHeader = (struct dbHeader *) malloc(sizeof(struct dbHeader));
    if(dbHeader == NULL){
        printf("Malloc failed.\n");
        return -1;
    }
    // update the content and align the endianness(from host to file/network)
    dbHeader->magic = htonl(header->magic);
    dbHeader->version = htons(header->version);
    dbHeader->count = htons(header->count);
    dbHeader->filesize = htonl(header->filesize);

    // prepare to write
    lseek(fd, SEEK_SET, 0);  // move cursor to the very front
    write(fd, dbHeader, sizeof(struct dbHeader));
    return 0;
}
