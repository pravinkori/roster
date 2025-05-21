#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n: create new database file\n");
    printf("\t -f: (required) path to database file\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    char *addstring = NULL;
    bool newfile = false;
    int c; // contain the current case that we are on

    int database_fd = -1;
    dbheader_t *database_header = NULL;
    employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:")) != -1) {
        switch (c) {
        case 'n':
            newfile = true;
            break;
        case 'f':
            filepath = optarg;
            break;
        case 'a':
            addstring = optarg;
            break;
        case '?':
            printf("Unknown option -%c\n", c);
            break;
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);
        return 0;
    }

    if (newfile) {
        database_fd = create_db_file(filepath);
        if (database_fd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }
        if (create_db_header(database_fd, &database_header) == STATUS_ERROR) {
            printf("Failed to create database header\n");
            return -1;
        }

    } else {
        database_fd = open_db_file(filepath);
        if (database_fd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }

        if (validate_db_header(database_fd, &database_header) == STATUS_ERROR) {
            printf("Failed to validate database header\n");
            return -1;
        }
    }

    printf("Newfile: %d\n", newfile);
    printf("File path: %s\n", filepath);

    if (read_employees(database_fd, database_header, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees\n");
        return 0;
    }

    if (addstring) {
        database_header->count++;
        // Create room for next new employee
        employees = realloc(employees, database_header->count * (sizeof(employee_t)));

        add_employee(database_header, employees, addstring);
    }

    output_file(database_fd, database_header, employees);

    return 0;
}
