#ifndef DEF_h
#define DEF_h

#include <stdint.h>

#define PROTO_VER 1

// define states for the fds
typedef enum{
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_MSG,
    STATE_FINISHED
} fileState;

// define message types
typedef enum{
    MSG_INIT_REQ,
    MSG_INIT_RESP,
    MSG_ADD_REQ,
    MSG_ADD_RESP,
    MSG_DEL_REQ,
    MSG_DEL_RESP,
    MSG_LIST_REQ,
    MSG_LIST_RESP,
    MSG_UPDATE_REQ,
    MSG_UPDATE_RESP,
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


#endif
