#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    fprintf(stderr, "Usage: %s -f <database file> [OPTIONS]\n", argv[0]);
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "\t -f <file>               : (required) path to database file\n");
    fprintf(stderr, "\t -n                      : create new database file\n");
    fprintf(stderr, "\t -a <data>               : add employee (format: name,address,hours)\n");
    fprintf(stderr, "\t -r <name>               : remove employee (format: name)\n");
    fprintf(stderr, "\t -u <name>,<hours>       : update employee hours (format: name,hours)\n");
    fprintf(stderr, "\t -l                      : list all employees\n");
    fprintf(stderr, "\t -h                      : show this help message\n");
    return;
}

int main(int argc, char *argv[]) {
    char *filepath = NULL;
    char *addstring = NULL;
    char *removename = NULL;
    char *updatestring = NULL;
    bool newfile = false;
    bool list = false;
    bool show_help = false;
    int c; // contain the current case that we are on

    int database_fd = -1;
    dbheader_t *database_header = NULL;
    employee_t *employees = NULL;

    while ((c = getopt(argc, argv, "nf:a:lr:u:h")) != -1) {
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
        case 'r':
            removename = optarg;
            break;
        case 'u':
            updatestring = optarg;
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

    if (!newfile && !list && !addstring && !removename && !updatestring) {
        fprintf(stderr, "Error: At least one operation must be specified (-n, -l, -a, -r, or -u)\n");
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
            fprintf(stderr, "Error: Unable to open database file '%s'\n", filepath);
            fprintf(stderr, "Hint: Use -n flag to create a new database\n");
            return -1;
        }

        if (validate_db_header(database_fd, &database_header) == STATUS_ERROR) {
            fprintf(stderr, "Error: Failed to validate database header for '%s'\n", filepath);
            fprintf(stderr, "The file may be corrupted or not a valid database\n");
            return -1;
        }
    }

    if (read_employees(database_fd, database_header, &employees) != STATUS_SUCCESS) {
        fprintf(stderr, "Error: Failed to read employees from database\n");
        return 0;
    }

    // Handle add employee operation
    if (addstring) {
        // Increment count first
        database_header->count++;

        // Reallocate memory for new employee
        employee_t *temp_employees = realloc(employees, database_header->count * sizeof(employee_t));
        if (temp_employees == NULL) {
            fprintf(stderr, "Error: Memory allocation failed when adding employee\n");
            database_header->count--; // Revert count increment
            return STATUS_ERROR;
        }
        employees = temp_employees;

        // Add the employee
        if (add_employee(database_header, employees, addstring) != STATUS_SUCCESS) {
            fprintf(stderr, "Error: Failed to add employee\n");
            database_header->count--; // Revert count increment
            return STATUS_ERROR;
        }

        printf("Employee added successfully\n");
    }

    if (removename) {
        if (remove_employee_by_name(database_header, &employees, removename) != STATUS_SUCCESS) {
            fprintf(stderr, "Error: Failed to remove employee '%s'\n", removename);
        } else {
            printf("Employee '%s' removed successfully\n", removename);
        }
    }

    if (updatestring) {
        char *sep = strchr(updatestring, ',');
        if (sep) {
            *sep = '\0';
            char *name = updatestring;
            int new_hours = atoi(sep + 1);
            if (update_employee_hours(database_header, employees, name, new_hours) != STATUS_SUCCESS) {
                fprintf(stderr, "Error: Failed to update hours for '%s'\n", name);
            } else {
                printf("Employee '%s' hours updated to %d\n", name, new_hours);
            }
        } else {
            fprintf(stderr, "Error: Invalid format for -u (expected name,hours)\n");
        }
    }

    if (list) {
        list_employees(database_header, employees);
    }

    // output_file(database_fd, database_header, employees);
    if (output_file(database_fd, database_header, employees) != STATUS_SUCCESS) {
        fprintf(stderr, "Error: Failed to write to database\n");
        free(employees);
        free(database_header);
        close(database_fd);
        return STATUS_ERROR;
    }

    free(employees);
    free(database_header);
    close(database_fd);

    return 0;
}
