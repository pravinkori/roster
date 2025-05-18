# roster

`roster` is a lightweight, Unix-style command-line utility for managing an employee database stored in a flat file. It supports viewing, adding, deleting, and updating employee records using flag-driven command-line options — making it ideal for scripting and automation.

## Features

- Load and validate an employee database from a file
- Add new employee entries
- Delete existing employees by key (e.g. name or address)
- Update employee fields (e.g. working hours)
- Create new databases if they don't exist
- Designed for ease of scripting — minimal interactivity

## Folder Structure

```
roster/  
├── src/ # Source files  
│ ├── main.c # Entry point and CLI parser  
│ ├── file.c # File I/O (open, close, create)  
│ └── parse.c # Parsing employee data  
├── include/ # Header files  
│ ├── main.h  
│ ├── file.h  
│ └── parse.h  
├── bin/ # Compiled binary output  
│ └── dbview  
├── obj/ # Object files  
├── Makefile # Build script  
├── .gitignore  
└── README.md

````

## Usage

### Build the project

```bash
make
````

### Run the utility

```sh
./bin/dbview [options]
```

### Options

| Flag         | Description                                    |
| ------------ | ---------------------------------------------- |
| `-f <file>`  | Specify the file path to database file to use  |
| `-n`         | Create a new database file if it doesn't exist |
| `-a <entry>` | Add an employee entry                          |
| `-d <key>`   | Delete an employee (by name or address)        |
| `-u <entry>` | Update employee details                        |

