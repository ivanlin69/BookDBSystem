#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "def.h"
#include "file.h"
#include "parse.h"
#include "server.h"

// Global array to store states of all clients
clientState clientStates[MAX_CLIENTS];

void printUsage(char *argv[]){
    printf("Usage: %s [options]\n", argv[0]);

    printf("\nLocal usage:\n");
    printf("  %s -n -f <database file>\n", argv[0]);
    printf("  %s -f <database file> -a <new book info>\n", argv[0]);
    printf("  %s -f <database file> -l\n", argv[0]);
    printf("  %s -f <database file> -r <book title>\n", argv[0]);
    printf("  %s -f <database file> -u <book title, year>\n", argv[0]);

    printf("\nOptions:\n");
    printf("  -n                Create new database file\n");
    printf("  -f <file>         Path to database file (required)\n");
    printf("  -a <info>         Add a new book with given info (title, author, genre, isbn)\n");
    printf("  -l                List all books in the database\n");
    printf("  -r <title>        Remove the book with the given title\n");
    printf("  -u <title,year>   Update the book's published year\n");

    printf("\nServer usage:\n");
    printf("  %s -f <database file> -p <port number>\n", argv[0]);
    printf("\nServer Options:\n");
    printf("  -p <port>         Port number to listen on (required)\n");
}

void pollLoop(unsigned short port, struct dbHeader *dbHeader, struct book **books, int dbfd){
    int listenFD, connectedFD, freeSlot;
    // all for ipv4
    struct sockaddr_in serverADDR = {0};
    struct sockaddr_in clientADDR = {0};
    socklen_t clientLen = sizeof(clientADDR);

    struct pollfd fds[MAX_CLIENTS + 1] = {0};
    int nfds = 1;
    int socketopt = 1;

    initialClients(clientStates);

    listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFD == -1){
        perror("listening socket");
        exit(EXIT_FAILURE);
    }
    // set the SO_REUSEADDR to true, which allows a socket to bind to an address that is already in use
    // so the server can restart quickly
    if(setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &socketopt, sizeof(socketopt)) == -1){
        perror("set socket option");
        exit(EXIT_FAILURE);
    }
    // set up server addr
    serverADDR.sin_family = AF_INET;
    serverADDR.sin_addr.s_addr = INADDR_ANY;  // allow any ip to connect
    serverADDR.sin_port = htons(port);
    // bind
    if(bind(listenFD, (struct sockaddr *) &serverADDR, sizeof(struct sockaddr)) == -1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    //listen
    if(listen(listenFD, 10) == -1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("\nServer is listening on port: %d\n", port);
    // load the listening socket to fds, increment the count
    fds[0].fd = listenFD;
    fds[0].events = POLLIN;  // at least one incoming connection request waiting to be accepted.

    while(1){
        int count = poll(fds, nfds, -1);
        if(count == -1){
            perror("poll");
            exit(EXIT_FAILURE);
        }
        // revents contains a bitmask of the events that were detected
        // we use bitwise operation to check if it contains POLLIN(each event only occupies single digit)
        // Check for new connections on the listening socket
        if(fds[0].revents & POLLIN){
            connectedFD = accept(listenFD, (struct sockaddr*) &clientADDR, &clientLen);
            if(connectedFD == -1){
                perror("accept");
                continue; // keep listening
            }
            printf("\nNew connection from: %s, %d\n", inet_ntoa(clientADDR.sin_addr), ntohs(clientADDR.sin_port));

            freeSlot = findEmptySlot(clientStates);
            if(freeSlot == -1){
                printf("No space for new connection.\n");
                close(connectedFD);
            } else {
                clientStates[freeSlot].fd = connectedFD;
                clientStates[freeSlot].state = STATE_CONNECTED;
                nfds++;
                printf("Slot %d has fd %d\n", freeSlot, clientStates[freeSlot].fd);
                fds[freeSlot + 1].fd = connectedFD; // Offset by 1 for listenFD
                fds[freeSlot + 1].events = POLLIN;
            }
        }
        // check each fds(clients) for activities
        for(int i=1; i<nfds; i++){
            if(fds[i].fd != -1 && fds[i].revents & POLLIN){
                int fd = fds[i].fd;
                int slot = findSlotByFD(fd, clientStates);
                // read the data to the corresponding buffer for further use
                ssize_t readData = read(fd, clientStates[slot].buffer, sizeof(clientStates[slot].buffer));
                if(readData <= 0){
                    if(readData == -1){
                        perror("read");
                    }
                    close(fd);
                    fds[i].fd = -1;
                    clientStates[slot].fd = -1;
                    clientStates[slot].state = STATE_DISCONNECTED;
                    printf("Client disconnected.\n");
                } else {
                    handleClient(&clientStates[slot], dbfd, dbHeader, books);
                }
            }
        }
        // reset fds
        nfds = 1;
        for(int i=0; i<MAX_CLIENTS; i++){
            if(clientStates[i].fd != -1){
                fds[nfds].fd = clientStates[i].fd;
                fds[nfds].events = POLLIN;
                nfds++;
            }
        }
    }
}

int main (int argc, char *argv[]){
    char *filepath = NULL;

    int option = 0;
    char *addInfo = NULL;
    char *removeTitle = NULL;
    char *updateInfo = NULL;
    char *portNumber = NULL;

    int newfile = 0;
    int addFlag = 0;
    int listFlag = 0;
    int removeFlag = 0;
    int updateFlag = 0;
    int portFlag = 0;

    int dbfd = 0;
    struct dbHeader * dbheader = NULL;

    // parse the user arguments
    while((option = getopt(argc, argv, "nf:a:lr:u:p:")) != -1){
        switch(option){
            case 'n':
                newfile = 1;
                break;
            case 'f':
                filepath = optarg;  //optarg is declared globally within the getopt library
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
            case 'p':
                portNumber = optarg;
                portFlag = 1;
                break;
            case '?':
                printf("Invalid option -%c\n", option);
            default:
                return -1;
        }
    }

    if(filepath == NULL){
        printf("Lack an argument for filepath.\n");
        printUsage(argv);
        return 0;
    }

    if(newfile){
        dbfd = createDBFile(filepath);
        if(dbfd == -1){
            printf("Unable to create database file.\n");
            return -1;
        }
        // create a new dbheader
        if(createDBHeader(&dbheader) == -1){
            printf("Database header creation failed.\n");
            return -1;
        }
    } else{
        // if the file already exists
        dbfd = openDBFile(filepath);
        if(dbfd == -1){
            printf("Unable to open database file.\n");
            return -1;
        }
        // validate and update the header
        if(validateDBHeader(dbfd, &dbheader) == -1){
            printf("Database header validation failed.\n");
            return -1;
        }
    }

    struct book * books = NULL;
    if(readBooks(dbfd, dbheader, &books) == -1){
        printf("Read books failed.\n");
        return -1;
    }

    if(addFlag == 1){
        if(addInfo == NULL){
            printf("Lack of an argument for adding new book.\n");
            printUsage(argv);
            return 0;
        }
        if(addBook(dbheader, &books, addInfo) == -1){
            printf("Adding new book failed.\n");
            return -1;
        }
    }

    if(removeFlag == 1){
        if(removeTitle == NULL){
            printf("Lack of an argument for removing the book.\n");
            printUsage(argv);
            return 0;
        }

        if(removeBook(dbheader, &books, removeTitle) == -1){
            printf("Removing the book: %s failed.\n", removeTitle);
            return -1;
        }
    }

    if(updateFlag == 1){
        if(updateInfo == NULL){
            printf("Lack of an argument for updating the book.\n");
            printUsage(argv);
            return 0;
        }

        if(updateBookPY(dbheader, books, updateInfo) == -1){
            printf("Updating the book's published year failed.\n");
            return -1;
        }
    }

    if(listFlag == 1){
        listAllBooks(dbheader, books);
    }

    // write/update the dbfile
    if(outputDBFile(dbfd, dbheader, books) == -1){
        printf("Output failed.\n");
        return -1;
    }

    if(portFlag == 1){
        if(portNumber == NULL){
            printf("Lack of an argument for port number.\n");
            printUsage(argv);
            return 0;
        }

        int port = atoi(portNumber);
        if(port == 0){
            printf("Given port number is bad.\n");
            return 0;
        } else {
            pollLoop(port, dbheader, &books, dbfd);
        }
    }

    close(dbfd);
    return 0;
}
