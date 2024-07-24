#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "def.h"


void printUsage(char *argv[]){
    printf("Usage: %s -p <port number> -h ‹host ip> -a\n", argv[0]);
    printf("\t -a (required) info to the new book(title, author, genre, isbn)\n");
    printf("\t -l List all books in the detabase\n");
    printf("\t -r (required) title of the book to be removed\n");
    printf("\t -u (required) title of the book to be updated and the year\n");
}

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

int main (int argc, char *argv[]){

    int option = 0;
    char *portNumber = NULL;
    unsigned short port = 0;
    char *hostIP = NULL;
    char *addInfo = NULL;
    char *removeTitle = NULL;
    char *updateYear = NULL;

    int addFlag = 0;
    int listFlag = 0;
    int removeFlag = 0;
    int updateFlag = 0;

    while((option = getopt(argc, argv, "p:h:a:lr:u:")) != -1){
        switch(option){
            case 'p':
                portNumber = optarg;
                port = atoi(portNumber);
                break;
            case 'h':
                hostIP = optarg;
                break;
            case 'a':
                addInfo = optarg;
                addFlag = 1;
                break;
            case 'l':
                listFlag = 1;
                break;
            case 'r':
                removeTitle = optarg;
                removeFlag = 1;
                break;
            case 'u':
                updateYear = optarg;
                updateFlag = 1;
                break;
            case '?':
                printf("Invalid option -%c\n", option);
            default:
                return -1;
        }
    }

    if(portNumber == NULL){
        printf("Lack an argument for port.\n");
        printUsage(argv);
        return 0;
    }

    if(port == 0){
        printf("Invalid port number.\n");
        return 0;
    }

    if(hostIP == NULL){
        printf("Lack an argument for host ip.\n");
        printUsage(argv);
        return 0;
    }

    // connect to host
    int hostFD;
    // all for ipv4
    struct sockaddr_in serverADDR = {0};
    socklen_t serverLen = sizeof(serverADDR);

    hostFD = socket(AF_INET, SOCK_STREAM, 0);
    if(hostFD == -1){
        perror("socket");
        return -1;
    }
    // set up server addr
    serverADDR.sin_family = AF_INET;
    serverADDR.sin_addr.s_addr = inet_addr(hostIP);
    serverADDR.sin_port = htons(port);

    if(connect(hostFD, (struct sockaddr *) &serverADDR, serverLen) == -1){
        perror("connect");
        close(hostFD);
        return 0;
    }

    if(sendInit(hostFD) == -1){
        return -1;
    }


    if(addFlag == 1){
        if(addInfo == NULL){
            printf("Lack of an argument for adding new book.\n");
            printUsage(argv);
            return 0;
        }

    }

    if(removeFlag == 1){
        if(removeTitle == NULL){
            printf("Lack of an argument for removing the book.\n");
            printUsage(argv);
            return 0;
        }

    }

    if(updateFlag == 1){
        if(updateYear == NULL){
            printf("Lack of an argument for updating the book.\n");
            printUsage(argv);
            return 0;
        }

    }

    if(listFlag == 1){

    }
    close(hostFD);
    return 0;
}
