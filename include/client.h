#ifndef CLIENT_h
#define CLIENT_h

#include "def.h"
#include "parse.h"

#define BUFFER_SIZE 2048

int sendInit(int fd);
// send add, del or update request
int sendADUReq(int fd, char* info, protocolMSG state);
int sendListReq(int fd);

#endif
