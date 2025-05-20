#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

int create_db_header(int fd, dbheader_t **header_out) {
    dbheader_t *header = calloc(1, sizeof(dbheader_t));
    if (header == -1) {
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
