#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "file.h"
#include "parse.h"

void printUsage(char *argv[]){
    printf("Usage: %s -n -f ‹database file>\n", argv[0]);
    printf("\t -n create new database file\n");
    printf("\t -f (required) path to database file\n");

    printf("Usage: %s -f ‹database file> -a <new book info>\n", argv[0]);
    printf("\t -a (required) info to the new book(title, author, genre, isbn)\n");
}

int main (int argc, char *argv[]){
    char *filepath = NULL;
    int newfile = 0;
    int option = 0;
    char *addInfo = NULL;
    int addFlag = 0;

    int dbfd = 0;
    struct dbHeader * dbheader = NULL;

    // parse the user arguments, -n for new file, -f for filepath
    //  -a for adding an new book to the database
    while((option = getopt(argc, argv, "nf:a:")) != -1){
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
            printf("Lack an argument for adding new book.\n");
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

    // write/update the dbfile
    if(outputDBFile(dbfd, dbheader, books) == -1){
        printf("Output failed.\n");
        return -1;
    }

    // for testing
    printf("Newfile %d\n", newfile);
    printf("Filepath %s\n", filepath);
    printf("AddInfo %s\n", addInfo);


    return 0;
}
