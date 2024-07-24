#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "server.h"
#include "parse.h"

void initialClients(clientState * clients){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        clients[i].fd = -1; // -1 means a free slot
        clients[i].state = STATE_NEW;
        memset(&clients[i].buffer, 0, BUFFERSIZE);
    }
}

int findEmptySlot(clientState * clients){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        if(clients[i].fd == -1){ // -1 means a free slot
            return i;
        }
    }
    return -1;
}

int findSlotByFD(int fd, clientState * clients){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        if(clients[i].fd == fd){
            return i;
        }
    }
    return -1;
}

int sendError(clientState * client, protocolHd * phdr){
    phdr->type = MSG_ERROR;
    phdr->len = 0;
    phdr->type = htonl(phdr->type);
    phdr->len = htons(phdr->len);
    // write content(INIT, PROTO_VER)
    if(write(client->fd, phdr, sizeof(phdr)) <= 0){
        perror("write");
        return -1;
    }
    return 0;
}

int sendInitResp(clientState * client, protocolHd * phdr){
    phdr->type = MSG_INIT_RESP;
    phdr->len = 1;
    phdr->type = htonl(phdr->type);
    phdr->len = htons(phdr->len);

    protocolInitRESP *res = (protocolInitRESP *) &phdr[1];
    res->version = PROTO_VER;
    res->version = htons(res->version);
    // write content(INIT, PROTO_VER)
    if(write(client->fd, phdr, sizeof(phdr) + sizeof(protocolInitRESP)) <= 0){
        perror("write");
        return -1;
    }
    return 0;
}


void handleClient(clientState* client, struct dbHeader *dbHeader, struct book *books){
    protocolHd *phdr = (protocolHd *) client->buffer;

    phdr->type = ntohl(phdr->type);
    phdr->len = ntohs(phdr->len);

    if(client->state == STATE_CONNECTED){
        if(phdr->type != MSG_INIT_REQ || phdr->len != 1){
            printf("Type mismatched(MSG_INIT_REQ).\n");
        }
        protocolInitReq *init = (protocolInitReq *) &phdr[1];
        init->version = ntohs(init->version);
        if(init->version != PROTO_VER){
            printf("Protocal version mismatched.\n");
            sendError(client, phdr);
            return;
        }
        sendInitResp(client, phdr);
        return;
    }

    if(client->state == STATE_MSG){

    }

}
