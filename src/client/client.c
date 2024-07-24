#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include "def.h"
#include "parse.h"
#include "client.h"

int sendInit(int fd){
    if(fd == -1){
        printf("Bad file descriptor.\n");
        return -1;
    }
    char buffer[1024] = {0};
    // write protocal header(INIT_REQ)
    protocolHd *phdr = (protocolHd *) buffer;
    phdr->type = MSG_INIT_REQ;
    phdr->len = 1;
    phdr->type = htonl(phdr->type);
    phdr->len = htons(phdr->len);
    // write content(INIT, PROTO_VER)
    protocolInitReq *init = (protocolInitReq *) &phdr[1];
    init->version = PROTO_VER;
    init->version = htons(init->version);

    if(write(fd, buffer, sizeof(phdr) + sizeof(protocolInitReq)) <= 0){
        perror("write");
        close(fd);
        return -1;
    }
    // read the respondse from host
    if(read(fd, buffer, sizeof(buffer)) <= 0){
        perror("read");
        close(fd);
        return -1;
    }
    phdr->type = ntohl(phdr->type);
    phdr->len = ntohs(phdr->len);
    // check the response from host
    if(phdr->type == MSG_ERROR){
        printf("Protocal mismatched).\n");
        close(fd);
        return -1;
    }

    printf("Server connected.\n");
    return 0;
}

int sendAddReq(int fd, char* addInfo){
    if(fd == -1){
        printf("Bad file descriptor.\n");
        return -1;
    }
    char buffer[1024] = {0};
    // write protocal header(INIT_REQ)
    protocolHd *phdr = (protocolHd *) buffer;
    phdr->type = MSG_ADD_REQ;
    phdr->len = 1;
    phdr->type = htonl(phdr->type);
    phdr->len = htons(phdr->len);
    // write content(INIT, PROTO_VER)
    protocolAddReq *add = (protocolAddReq *) &phdr[1];
    // size is always the same as buffer to avoid overflow
    strncpy(add->data, addInfo, sizeof(add->data));

    if(write(fd, buffer, sizeof(protocolHd) + sizeof(protocolAddReq)) <= 0){
        perror("write");
        close(fd);
        return -1;
    }
    // read the respondse from host
    if(read(fd, buffer, sizeof(buffer)) <= 0){
        perror("read");
        close(fd);
        return -1;
    }
    phdr->type = ntohl(phdr->type);
    phdr->len = ntohs(phdr->len);
    // check the response from host
    if(phdr->type == MSG_ERROR){
        printf("Protocal mismatched.\n");
        close(fd);
        return -1;
    }

    if(phdr->type == MSG_ADD_RESP){
        printf("Book successfully added to the database.\n");
    }
    return 0;
}

int sendDelReq(int fd, char* updateInfo){

    return 0;
}


int sendUpdateReq(int fd, char* updateInfo){
     return 0;
}

int sendListReq(int fd){
    if(fd == -1){
        printf("Bad file descriptor.\n");
        return -1;
    }
    char buffer[1024] = {0};
    // write protocal header(INIT_REQ)
    protocolHd *phdr = (protocolHd *) buffer;
    phdr->type = MSG_LIST_REQ;
    phdr->len = 0;
    phdr->type = htonl(phdr->type);
    phdr->len = htons(phdr->len);

    if(write(fd, buffer, sizeof(protocolHd)) <= 0){
        perror("write");
        close(fd);
        return -1;
    }
    // read the respondse from host
    if(read(fd, phdr, sizeof(protocolHd)) <= 0){
        perror("read");
        close(fd);
        return -1;
    }
    phdr->type = ntohl(phdr->type);
    phdr->len = ntohs(phdr->len);
    // check the response from host
    if(phdr->type == MSG_ERROR){
        printf("Unable to aquire data).\n");
        close(fd);
        return -1;
    }

    if(phdr->type == MSG_LIST_RESP){
        printf("List received.\n");
        protocolListRESP *book = (protocolListRESP *) &phdr[1];

        for(int i=0; i<phdr->len; i++){
            if(read(fd, book, sizeof(protocolListRESP)) <= 0){
                perror("read");
                close(fd);
                return -1;
            }
            //book->publishedYear = ntohs(book->publishedYear);
            printf("\nBook %d:\n", i+1);
            // %-15s left-justified within a field of 15 characters
            // The - sign indicates left-justification, remove it for right-justification
            printf("%15s: %s\n", "Title", book->title);
            printf("%15s: %s\n", "Author", book->author);
            printf("%15s: %s\n", "Genre", book->genre);
            printf("%15s: %s\n", "ISBN", book->isbn);
            printf("%15s: %d\n", "Published year", ntohs(book->publishedYear));
        }
    }
    return 0;
}
