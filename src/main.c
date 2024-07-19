#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "file.h"

void printUsage(char *argv[]){
    printf("Usage: %s -n -f ‹database file>\n", argv[0]);
    printf("\t -n create new database file\n");
    printf("\t -f (required) path to database file\n");
}

int main (int argc, char *argv[]){

    char *filepath = NULL;
    bool newfile = false;
    bool list = false;
    int option = 0;

    while((option = getopt(argc, argv, "nf:a:l")) != -1){
        switch(option){
            case 'n':
                newfile = true;
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
        int dbfd = createDBFile(filepath);
        if(dbfd == -1){
            printf("Unable to create database file.\n");
            return -1;
        }
    } else{
        int dbfd = openDBFile(filepath);
        if(dbfd == -1){
            printf("Unable to open database file.\n");
            return -1;
        }
    }
    printf("Newfile %d\n", newfile);
    printf("Filepath %s\n", filepath);

    return 0;
}
