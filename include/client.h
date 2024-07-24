#ifndef CLIENT_h
#define CLIENT_h

#include "def.h"
#include "parse.h"

int sendInit(int fd);
int sendAddReq(int fd, char* addInfo);

#endif
