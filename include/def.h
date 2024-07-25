#ifndef DEF_h
#define DEF_h

#include <stdint.h>

#define PROTO_VER 1

// define states for the fds
typedef enum{
    STATE_NEW,
    STATE_CONNECTED,
    STATE_MSG,
    STATE_DISCONNECTED,
} fileState;

// define message types
typedef enum{
    MSG_INIT_REQ,
    MSG_INIT_RESP,
    MSG_ADD_REQ,
    MSG_ADD_RESP,
    MSG_DEL_REQ,
    MSG_DEL_RESP,
    MSG_UPDATE_REQ,
    MSG_UPDATE_RESP,
    MSG_LIST_REQ,
    MSG_LIST_RESP,
    MSG_ERROR
} protocolMSG;

typedef struct{
    protocolMSG type;
    uint16_t len;
} protocolHd;  // protocal's header

typedef struct{
    uint16_t version;
} protocolInitReq;  // in the init state, we only check the version of the proto

typedef struct{
    uint16_t version;
} protocolInitRESP;

// used for ADD, DEL and UPDATE request
typedef struct{
    char data[1024];
} protocolADUReq;

typedef struct{
    char title[32];
    char author[32];
    char genre[32];
    char isbn[16];
    uint16_t publishedYear;
} protocolListRESP;

#endif
