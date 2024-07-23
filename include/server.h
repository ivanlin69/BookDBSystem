#ifndef SERVER_h
#define SERVER_h

#include <poll.h>

#define MAX_CLIENTS 20
#define PORT 8080
#define BUFFERSIZE 1024

typedef enum{
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED
} fileState;

typedef struct{
    int fd;
    fileState state;
    char buffer[BUFFERSIZE];
} clientState;

void initialClients(clientState * states);
int findEmptySlot(clientState * states);
int findSlotByFD(int fd, clientState * states);


#endif
