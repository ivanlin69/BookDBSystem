#ifndef CLIENT_h
#define CLIENT_h

#include "def.h"
#include "parse.h"

int sendInit(int fd);
int sendAddReq(int fd, char* addInfo);
int sendDelReq(int fd, char* removeInfo);
int sendUpdateReq(int fd, char* updateInfo);
int sendListReq(int fd);

#endif
