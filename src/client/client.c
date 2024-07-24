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
    printf("content: %s\n", addInfo);
    // size is always the same as buffer to avoid overflow
    strncpy(add->data, addInfo, sizeof(add->data));

    printf("content: %s\n", add->data);

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
        printf("Protocal mismatched).\n");
        close(fd);
        return -1;
    }

    if(phdr->type == MSG_ADD_RESP){
        printf("Book successfully added to the database.\n");
    }
    return 0;
}
