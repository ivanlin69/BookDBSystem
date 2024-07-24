#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/_endian.h>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/syslimits.h>
#include <sys/time.h>
#include <unistd.h>

#include "def.h"
#include "file.h"
#include "parse.h"
#include "server.h"

// Global array to store states of all clients
clientState clientStates[MAX_CLIENTS];

void printUsage(char *argv[]){
    printf("Usage: %s -n -f ‹database file>\n", argv[0]);
    printf("\t -n create new database file\n");
    printf("\t -f (required) path to database file\n");

    printf("Usage: %s -f ‹database file> -a <new book info>\n", argv[0]);
    printf("\t -a (required) info to the new book(title, author, genre, isbn)\n");

    printf("Usage: %s -f ‹database file> -l\n", argv[0]);
    printf("\t -l List all books in the detabase\n");

    printf("Usage: %s -f ‹database file> -r <book title>\n", argv[0]);
    printf("\t -r (required) title of the book to be removed\n");

    printf("Usage: %s -f ‹database file> -u <book title, year>\n", argv[0]);
    printf("\t -u (required) title of the book to be updated and the year\n");

    printf("Usage: %s -f ‹database file> -p <port number>\n", argv[0]);
    printf("\t -p (required) port number to listen to\n");
}

void pollLoop(unsigned short port, struct dbHeader *dbHeader, struct book *books){
    int listenFD;
    int connectedFD;
    int freeSlot;
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
    serverADDR.sin_port = htons(PORT);

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

    printf("Server is listening on port: %d\n", PORT);
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
            printf("New connection: %s, %d\n", inet_ntoa(clientADDR.sin_addr), ntohs(clientADDR.sin_port));

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

                ssize_t readData = read(fd, clientStates[slot].buffer, sizeof(clientStates[slot].buffer));
                if(readData <= 0){
                    perror("read");
                    close(fd);
                    fds[i].fd = -1;
                    clientStates[slot].fd = -1;
                    clientStates[slot].state = STATE_DISCONNECTED;
                    printf("Client disconnected.\n");
                } else {
                    /**
                    clientStates[slot].buffer[readData] = '\0';
                    printf("Data from the client: %s \n", clientStates[slot].buffer);
                     */
                    handleClient(&clientStates[slot], dbHeader, books);
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
    int newfile = 0;
    int option = 0;
    char *addInfo = NULL;
    char *removeTitle = NULL;
    char *updateYear = NULL;
    char *portNumber = NULL;
    int addFlag = 0;
    int listFlag = 0;
    int removeFlag = 0;
    int updateFlag = 0;
    int portFlag = 0;

    int dbfd = 0;
    struct dbHeader * dbheader = NULL;

    // parse the user arguments, -n for new file, -f for filepath
    //  -a for adding an new book to the database
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
                updateYear = optarg;
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

        //allocate spaces for new book and update the info header
        dbheader->count++;
        books = (struct book*) realloc(books, (dbheader->count)*sizeof(struct book));
        if(books == NULL){
            printf("realloc failed.\n");
            return -1;
        }

        if(addBook(dbheader, books, addInfo) == -1){
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
        if(updateYear == NULL){
            printf("Lack of an argument for updating the book.\n");
            printUsage(argv);
            return 0;
        }

        if(updateBookPY(dbheader, books, updateYear) == -1){
            printf("Updating the book's published year failed.\n");
            return -1;
        }
    }

    if(listFlag == 1){
        listAllBooks(dbheader, books);
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
            pollLoop(port, dbheader, books);
        }
    }

    // write/update the dbfile
    if(outputDBFile(dbfd, dbheader, books) == -1){
        printf("Output failed.\n");
        return -1;
    }

    /**
    printf("\nFor testing:\n");
    printf("Newfile %d\n", newfile);
    printf("Filepath %s\n", filepath);
    printf("AddInfo %s\n", addInfo);
    */

    close(dbfd);
    return 0;
}
