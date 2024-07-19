#include <stdio.h>
#include <getopt.h>

#include "file.h"
#include "parse.h"

void printUsage(char *argv[]){
    printf("Usage: %s -n -f ‹database file>\n", argv[0]);
    printf("\t -n create new database file\n");
    printf("\t -f (required) path to database file\n");
}

int main (int argc, char *argv[]){
    char *filepath = NULL;
    int newfile = 0;
    int option = 0;

    int dbfd = 0;
    struct dbHeader * dbheader = NULL;

    // parse the user arguments, -n for new file, -f for filepath
    while((option = getopt(argc, argv, "nf:")) != -1){
        switch(option){
            case 'n':
                newfile = 1;
                break;
            case 'f':
                filepath = optarg;  //optarg is declared globally within the getopt library
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

    // write/update the dbfile
    if(outputDBFile(dbfd, dbheader) == -1){
        printf("Output failed.\n");
        return -1;
    }

    // for testing
    printf("Newfile %d\n", newfile);
    printf("Filepath %s\n", filepath);

    return 0;
}
