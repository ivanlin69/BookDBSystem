#ifndef PARSE_h
#define PARSE_h

#define HEADER_ MAGIC 0x4c4c4144

struct dbHeader {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct book{
    char title[256];
    char author[256];
    char *genre[10];
    unsigned int publishYear;
};

int createDBHeader(int fd, struct dbHeader **headerOut);
int validateDBHeader(int fd, struct dbHeader **headerOut);
int readBook (int fd, struct dbHeader **headerOut, struct book **bookOut);


#endif
