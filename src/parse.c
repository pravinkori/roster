#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

int create_db_header(int fd, dbheader_t **header_out) {
    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == NULL) {
        printf("Memory allocation failed to create database header\n");
        return STATUS_ERROR;
    }

    header->version = 0x1;
    header->count = 0;
    header->signature = DB_HEADER_SIGNATURE;
    header->filesize = sizeof(dbheader_t);

    *header_out = header;

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, dbheader_t **header_out) {
    if (fd < 0) {
        printf("Bad fd from the user\n");
        return STATUS_ERROR;
    }

    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == NULL) {
        printf("Memory allocation failed to create database header\n");
        return STATUS_ERROR;
    }

    if (read(fd, header, sizeof(dbheader_t)) != sizeof(dbheader_t)) {
        perror("read");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->signature = ntohl(header->signature);
    header->filesize = ntohl(header->filesize);

    if (header->version != 1) {
        printf("Improper header version\n");
        free(header);
        return -1;
    }

    if (header->signature != DB_HEADER_SIGNATURE) {
        printf("Improper header signature\n");
        free(header);
        return -1;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size) {
        printf("Corrupted database");
        free(header);
        return -1;
    }

    return STATUS_SUCCESS;
}

void output_file(int fd, dbheader_t *database_header) {
    if (fd < 0) {
        printf("Bad fd from the user\n");
        return STATUS_ERROR;
    }

    database_header->signature = htonl(database_header->signature);
    database_header->filesize = htonl(database_header->filesize);
    database_header->count = htons(database_header->count);
    database_header->version = htons(database_header->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, database_header, sizeof(dbheader_t));

    return;
}
