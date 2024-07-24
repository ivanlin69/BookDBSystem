#ifndef SERVER_h
#define SERVER_h

#include <poll.h>
#include "def.h"
#include "parse.h"

#define MAX_CLIENTS 20
#define PORT 8080
#define BUFFERSIZE 1024

typedef struct{
    int fd;
    fileState state;
    char buffer[BUFFERSIZE];
} clientState;

void initialClients(clientState * clients);
int findEmptySlot(clientState * clients);
int findSlotByFD(int fd, clientState * clients);
void handleClient(clientState * client, int dbfd, struct dbHeader *dbHeader, struct book **books);
int sendError(clientState * client, protocolHd * phdr);
int sendInitResp(clientState * client, protocolHd * phdr);
int sendAddResp(clientState * client, protocolHd * phdr);

#endif
