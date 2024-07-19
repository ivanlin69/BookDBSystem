#ifndef PARSE_h
#define PARSE_h

#define HEADER_MAGIC 0x09050207

struct dbHeader {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct book{
    char title[32];
    char author[32];
    char *genre[5];
    unsigned short publishYear;
    unsigned short isbn;
};

int createDBHeader(struct dbHeader **outputHeader);
int validateDBHeader(int fd, struct dbHeader **outputHeader);
int outputDBFile(int fd, struct dbHeader *header);


#endif
