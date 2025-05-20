#ifndef PARSE_H
#define PARSE_H

#define DB_HEADER_SIGNATURE 0x5052494D45

typedef struct DBHeader {
    unsigned int signature;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
} dbheader_t;

int create_db_header(int fd, dbheader_t **header_out);

#endif