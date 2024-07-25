#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "def.h"
#include "client.h"


void printUsage(char *argv[]){
    printf("Usage: %s [options]\n", argv[0]);

    printf("\nOptions:\n");
    printf("  -p <port number>      Specify the port number to connect to (required)\n");
    printf("  -h <host ip>          Specify the host IP address to connect to (required)\n");
    printf("  -a <book info>        Add a new book with the given info (title, author, genre, isbn)\n");
    printf("  -l                    List all books in the database\n");
    printf("  -r <book title>       Remove the book with the given title\n");
    printf("  -u <title, year>      Update the published year of the book with the given title\n");

    printf("\nExamples:\n");
    printf("  %s -p 8080 -h 127.0.0.1 -a 'Title,Author,Genre,ISBN'\n", argv[0]);
    printf("  %s -p 8080 -h 127.0.0.1 -l\n", argv[0]);
    printf("  %s -p 8080 -h 127.0.0.1 -r 'Title'\n", argv[0]);
    printf("  %s -p 8080 -h 127.0.0.1 -u 'Title,Year'\n", argv[0]);
}


int main (int argc, char *argv[]){

    int option = 0;
    char *portNumber = NULL;
    char *hostIP = NULL;
    char *addInfo = NULL;
    char *removeTitle = NULL;
    char *updateInfo = NULL;

    unsigned short port = 0;
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
                updateInfo = optarg;
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
    // Sending request for initialization
    printf("\nSending request for validating protocal ...\n");
    if(sendInit(hostFD) == -1){
        close(hostFD);
        return -1;
    }

    if(addFlag == 1){
        if(addInfo == NULL){
            printf("Lack of an argument for adding new book.\n");
            printUsage(argv);
            close(hostFD);
            return 0;
        }
        // send request for adding a new book
        printf("\nSending request for adding a new book ...\n");
        if(sendADUReq(hostFD, addInfo, MSG_ADD_REQ) == -1){
            close(hostFD);
            return -1;
        }
    }

    if(removeFlag == 1){
        if(removeTitle == NULL){
            printf("Lack of an argument for removing the book.\n");
            printUsage(argv);
            close(hostFD);
            return 0;
        }
        removeTitle[strlen(removeTitle)] = '\0';
        // send request for removing a book
        printf("\nSending request for removing the book ...\n");
        if(sendADUReq(hostFD, removeTitle, MSG_DEL_REQ) == -1){
            close(hostFD);
            return -1;
        }
    }

    if(updateFlag == 1){
        if(updateInfo == NULL){
            printf("Lack of an argument for updating the book.\n");
            printUsage(argv);
            close(hostFD);
            return 0;
        }
        updateInfo[strlen(updateInfo)] = '\0';
        // send request for updating a book's published year'
        printf("\nSending request for updating the book ...\n");
        if(sendADUReq(hostFD, updateInfo, MSG_UPDATE_REQ) == -1){
            close(hostFD);
            return -1;
        }
    }

    if(listFlag == 1){
        // send request for listing all books
        printf("\nSending request for listing all books in the database ...\n");
        if(sendListReq(hostFD) == -1){
            close(hostFD);
            return -1;
        }
    }
    close(hostFD);
    return 0;
}
