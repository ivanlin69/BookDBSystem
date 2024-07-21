#ifndef PARSE_h
#define PARSE_h

#define HEADER_MAGIC 0x4956414E

struct dbHeader {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct book{
    char title[32];
    char author[32];
    char genre[32];
    char isbn[16];
    unsigned short publishedYear;

};

int createDBHeader(struct dbHeader **outputHeader);
int validateDBHeader(int fd, struct dbHeader **outputHeader);
int readBooks(int fd, struct dbHeader *dbheader, struct book **outputBooks);
int addBook(struct dbHeader *dbheader, struct book *books, char *title);
int removeBook(struct dbHeader *dbheader, struct book *books, char *info);
int outputDBFile(int fd, struct dbHeader *header, struct book* books);
void listAllBooks(struct dbHeader *header, struct book* books);


#endif
