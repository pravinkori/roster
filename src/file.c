#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "file.h"

int create_db_file(char *filename) {
    // check if file already exist
    int fd = open(filename, O_RDONLY);
    if (fd != -1) {
        close(fd);
        printf("File already exists\n");
        return STATUS_ERROR;
    }
    // if specified file doesn't exist, create the file
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }
    printf("File '%s' created successfully.\n", filename);

    return fd;
}

int open_db_file(char *filename) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        return STATUS_ERROR;
    }
    // printf("file opened\n");
    return fd;
}
