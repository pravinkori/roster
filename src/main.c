#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    fprintf(stderr, "Usage: %s -f <database file> [OPTIONS]\n", argv[0]);
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "\t -f <file>     : (required) path to database file\n");
    fprintf(stderr, "\t -n            : create new database file\n");
    fprintf(stderr, "\t -a <data>     : add employee (format: name,address,hours)\n");
    fprintf(stderr, "\t -l            : list all employees\n");
    fprintf(stderr, "\t -h            : show this help message\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    char *addstring = NULL;
    bool newfile = false;
    bool list = false;
    bool show_help = false;
    int c; // contain the current case that we are on

    int database_fd = -1;
    dbheader_t *database_header = NULL;
    employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:lh")) != -1) {
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
        case 'l':
            list = true;
            break;
        case 'h':
            show_help = true;
            break;
        case '?':
            fprintf(stderr, "Unknown option -%c\n", optopt);
            print_usage(argv);
            break;
        default:
            fprintf(stderr, "Unexpected error in argument parsing\n");
            return -1;
        }
    }

    if (show_help) {
        print_usage(argv);
        return STATUS_SUCCESS;
    }

    if (filepath == NULL) {
        fprintf(stderr, "Error: Filepath is a required argument\n");
        print_usage(argv);
        return 0;
    }

    if (!newfile && !list && !addstring) {
        fprintf(stderr, "Error: At least one operation must be specified (-n, -l, or -a)\n");
        print_usage(argv);
        return EXIT_FAILURE;
    }

    if (newfile) {
        database_fd = create_db_file(filepath);
        if (database_fd == STATUS_ERROR) {
            fprintf(stderr, "Error: Unable to create database file '%s'\n", filepath);
            return -1;
        }

        if (create_db_header(database_fd, &database_header) == STATUS_ERROR) {
            fprintf(stderr, "Error: Failed to create database header\n");
            return -1;
        }

        printf("Successfully created new database: %s\n", filepath);

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

    if (list) {
        list_employees(database_header, employees);
    }

    output_file(database_fd, database_header, employees);

    // TODO:
    // 1. Remove an employee by name:
    //      - find employee by name and remove them using -r
    // 2. Update the hours of employee:
    //      - find employee by name and remove them using -r

    return 0;
}
