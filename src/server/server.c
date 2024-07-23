#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <poll.h>

#include "server.h"

void initialClients(clientState * states){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        states[i].fd = -1; // -1 means a free slot
        states[i].state = STATE_NEW;
        memset(&states[i].buffer, 0, BUFFERSIZE);
    }
}

int findEmptySlot(clientState * states){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        if(states[i].fd == -1){ // -1 means a free slot
            return i;
        }
    }
    return -1;
}

int findSlotByFD(int fd, clientState * states){
    for(size_t i=0; i<MAX_CLIENTS; i++){
        if(states[i].fd == fd){
            return i;
        }
    }
    return -1;
}
