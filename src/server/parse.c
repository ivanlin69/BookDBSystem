#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parse.h"

int createDBHeader(struct dbHeader **outputHeader){
    struct dbHeader * newHeader = (struct dbHeader *) malloc(sizeof(struct dbHeader));
    if(newHeader == NULL){
        printf("Malloc failed.\n");
        return -1;
    }
    newHeader->magic = HEADER_MAGIC;
    newHeader->version = 1;
    newHeader->count = 0;
    newHeader->filesize = sizeof(struct dbHeader);

    *outputHeader = newHeader;
    return 0;
}

int validateDBHeader(int fd, struct dbHeader **outputHeader){
    if(fd < 0){
        printf("Invalid file descriptor.\n");
        return -1;
    }

    struct dbHeader * header = (struct dbHeader *) malloc(sizeof(struct dbHeader));
    if(header == NULL){
        printf("Malloc failed.\n");
        return -1;
    }
    // read the given header to the buffer, make sure the read size is expected
    if(read(fd, header, sizeof(struct dbHeader)) !=  sizeof(struct dbHeader)){
        perror("read");
        free(header);
        return -1;
    }
    // update the content and align the endianness(from file/network to host)
    header->magic = ntohl(header->magic);
    if(header->magic != HEADER_MAGIC){
        printf("Invalid header magic.\n");
        free(header);
        return -1;
    }
    header->version = ntohs(header->version);
    if(header->version != 1){
        printf("Invalid header version.\n");
        free(header);
        return -1;
    }

    header->count = ntohs(header->count);
    header->filesize = ntohl(header->filesize);

    // use stat to aquire the size of the given file
    struct stat dbStat = {0};
    if(fstat(fd, &dbStat) == -1){
        perror("fstat");
        free(header);
        return -1;
    }
    // make sure the size recorded in the header is the same as its real size
    if(header->filesize != dbStat.st_size){
        printf("Invalid header size.\n");
        free(header);
        return -1;
    }

    *outputHeader = header;
    return 0;
}

int readBooks(int fd, struct dbHeader *dbheader, struct book **outputBooks){
    if(fd < 0){
        printf("Invalid file descriptor.\n");
        return -1;
    }    unsigned short count = dbheader->count;
    struct book * books = (struct book *) calloc(count, sizeof(struct book));
    if(books == NULL){
        printf("Calloc failed.\n");
        return -1;
    }

    if(read(fd, books, sizeof(struct book)*count) != sizeof(struct book)*count){
        perror("read");
        free(books);
        return -1;
    }

    for(size_t i=0; i<count; i++){
        books[i].publishedYear = ntohs(books[i].publishedYear);
    }
    *outputBooks = books;
    return 0;
}

int addBook(struct dbHeader *dbheader, struct book **books, char* info){

    char* title = strtok(info, ",");
    if(title == NULL){
        return -1;
    }
    char* author = strtok(NULL, ",");
    if(author == NULL){
        return -1;
    }
    char* genre = strtok(NULL, ",");
    if(genre == NULL){
        return -1;
    }
    char* isbn = strtok(NULL, ",");
    if(isbn == NULL){
        return -1;
    }
    char* publishedYear = strtok(NULL, ",");
    if(publishedYear == NULL){
        return -1;
    }

    //allocate spaces for new book and update the info header
    dbheader->count++;
    struct book* newBooks = (struct book*) realloc(*books, (dbheader->count)*sizeof(struct book));
    if(newBooks == NULL){
        printf("realloc failed.\n");
        return -1;
    }
    *books = newBooks;
    int count = dbheader->count;

    strncpy((*books)[count-1].title, title, sizeof((*books)[count-1].title) - 1);
    strncpy((*books)[count-1].author, author, sizeof((*books)[count-1].author) - 1);
    strncpy((*books)[count-1].genre, genre, sizeof((*books)[count-1].genre) - 1);
    strncpy((*books)[count-1].isbn, isbn, sizeof((*books)[count-1].isbn) - 1);
    (*books)[count-1].publishedYear = atoi(publishedYear);

    // for ensuring null-terminatio
    (*books)[count-1].title[sizeof((*books)[count-1].title) - 1] = '\0';
    (*books)[count-1].author[sizeof((*books)[count-1].author) - 1] = '\0';
    (*books)[count-1].genre[sizeof((*books)[count-1].genre) - 1] = '\0';
    (*books)[count-1].isbn[sizeof((*books)[count-1].isbn) - 1] = '\0';

    /**
    printf("\nTesting for info:\n");
    printf("%s  %s  %s  %s  %d  \n",
        (*books)[dbheader->count-1].title,
        (*books)[dbheader->count-1].author,
        (*books)[dbheader->count-1].genre,
        (*books)[dbheader->count-1].isbn,
        (*books)[dbheader->count-1].publishedYear);
        */
    return 0;
}

int removeBook(struct dbHeader *dbheader, struct book **books, char *title){

    int index=-1;
    for(int i=0; i<dbheader->count; i++){
        if(strcmp((*books)[i].title, title) == 0){
            index = i;
            break;
        }
    }
    if(index == -1){
        printf("Book not found.\n");
        return -1;
    }
    for(int i=index; i<dbheader->count-1; i++){
        (*books)[i] = (*books)[i+1];
    }
    dbheader->count--;
    struct book *newBooks = (struct book *) realloc(*books, (dbheader->count)*sizeof(struct book));
    if(newBooks == NULL){
        printf("realloc failed.\n");
        return -1;
    }
    *books = newBooks;
    return 0;
}

int updateBookPY(struct dbHeader *dbheader, struct book *books, char *info){

    char* title = strtok(info, ",");
    if(title == NULL){
        return -1;
    }
    unsigned short newYear = atoi(strtok(NULL, ","));
    if(newYear == 0){
        return -1;
    }

    int index=-1;
    for(int i=0; i<dbheader->count; i++){
        if(strcmp(books[i].title, title) == 0){
            index = i;
            break;
        }
    }
    if(index == -1){
        printf("Book not found.\n");
        return 0;
    }
    books[index].publishedYear = newYear;
    return 0;
}

int outputDBFile(int fd, struct dbHeader *header, struct book* books){
    if(fd < 0){
        printf("Invalid file descriptor.\n");
        return -1;
    }
    unsigned short count = header->count;
    struct dbHeader outputHeader = {0};
    // update the content and align the endianness(from host to file/network)
    outputHeader.magic = htonl(header->magic);
    outputHeader.version = htons(header->version);
    outputHeader.count = htons(header->count);
    outputHeader.filesize = htonl(sizeof(struct dbHeader)+sizeof(struct book)*count);

    // prepare to write
    lseek(fd, 0, SEEK_SET);  // move cursor to the very front
    if(write(fd, &outputHeader, sizeof(struct dbHeader)) != sizeof(struct dbHeader)){
        perror("write header");
        return -1;
    }

    if(books){
        for(unsigned short i=0; i<count; i++){
            books[i].publishedYear = htons(books[i].publishedYear);
        }
        if(write(fd, books, count*sizeof(struct book)) != count*sizeof(struct book)){
            perror("write books");
            return -1;
        }
    }

    // for further use, we resume the state of the file
    for(unsigned short i=0; i<count; i++){
        books[i].publishedYear = ntohs(books[i].publishedYear);
    }
    // Truncate the file to the new size if needed
    if (ftruncate(fd, sizeof(struct dbHeader) + sizeof(struct book) * count) == -1) {
        perror("ftruncate");
        return -1;
    }

    return 0;
}

void listAllBooks(struct dbHeader *header, struct book* books){
    for(unsigned short i=0; i<header->count; i++){
        printf("\nBook %d:\n", i+1);
        // %-15s left-justified within a field of 15 characters
        // The - sign indicates left-justification, remove it for right-justification
        printf("%15s: %s\n", "Title", books[i].title);
        printf("%15s: %s\n", "Author", books[i].author);
        printf("%15s: %s\n", "Genre", books[i].genre);
        printf("%15s: %s\n", "ISBN", books[i].isbn);
        printf("%15s: %d\n", "Published year", books[i].publishedYear);
    }
}
