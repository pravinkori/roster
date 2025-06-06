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
    if (fd < 0) {
        fprintf(stderr, "Error: Invalid file descriptor\n");
        return STATUS_ERROR;
    }

    if (header_out == NULL) {
        fprintf(stderr, "Error: NULL pointer passed for header output\n");
        return STATUS_ERROR;
    }

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
        printf("Corrupted database\n");
        free(header);
        return -1;
    }

    *header_out = header;

    return STATUS_SUCCESS;
}

int output_file(int fd, dbheader_t *database_header, employee_t *employees) {
    if (fd < 0) {
        printf("Bad fd from the user\n");
        return STATUS_ERROR;
    }

    int realcount = database_header->count;

    database_header->signature = htonl(database_header->signature);
    database_header->filesize = htonl(sizeof(dbheader_t) + sizeof(employee_t) * realcount);
    database_header->count = htons(database_header->count);
    database_header->version = htons(database_header->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, database_header, sizeof(dbheader_t));

    int i = 0;
    for (i = 0; i < realcount; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(employee_t));
    }

    return STATUS_SUCCESS;
}

int read_employees(int fd, dbheader_t *database_header, employee_t **employees_out) {
    if (fd < 0) {
        printf("Bad fd from the user\n");
        return STATUS_ERROR;
    }

    int count = database_header->count;
    size_t total_size = count * sizeof(employee_t);

    employee_t *employees = calloc(count, sizeof(employee_t));
    if (employees == NULL) {
        printf("Memory allocation failed\n");
        return STATUS_ERROR;
    }

    // read(fd, employees, count * sizeof(employee_t));

    // Read employee data from file
    ssize_t bytes_read = read(fd, employees, total_size);
    if (bytes_read < 0) {
        perror("Error reading employees from file");
        free(employees);
        return STATUS_ERROR;
    }

    // Check if we read exactly the expected number of bytes
    if ((size_t)bytes_read != total_size) {
        printf("Incomplete read: expected %zu bytes, got %zd bytes\n", total_size, bytes_read);
        free(employees);
        return STATUS_ERROR;
    }

    int i = 0;
    for (; i < count; i++) {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employees_out = employees;
    return STATUS_SUCCESS;
}

int add_employee(dbheader_t *database_header, employee_t *employees, char *addstring) {
    printf("%s\n", addstring);

    char *name = strtok(addstring, ",");
    char *address = strtok(NULL, ",");
    char *hours = strtok(NULL, ",");

    strncpy(employees[database_header->count - 1].name, name, sizeof(employees[database_header->count - 1].name));
    strncpy(employees[database_header->count - 1].address, address, sizeof(employees[database_header->count - 1].address));
    employees[database_header->count - 1].hours = atoi(hours);

    printf("%s %s %s\n", name, address, hours);
    return STATUS_SUCCESS;
}

void list_employees(dbheader_t *database_header, employee_t *employees) {
    int i = 0;
    for (i = 0; i < database_header->count; i++) {
        printf("Employee: %d\n", i + 1);
        printf("\tName: %s\n", employees[i].name);
        printf("\tAddress: %s\n", employees[i].address);
        printf("\tHours: %d\n\n", employees[i].hours);
    }
    printf("Total %d employee%s\n", database_header->count,
           database_header->count == 1 ? "" : "s");
}
