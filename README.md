# roster

`roster` (binary: `roster`) is a lightweight, Unix-style command-line utility for managing an employee database stored in a flat file. It supports viewing, adding, deleting, and updating employee records using flag-driven command-line options — making it ideal for scripting and automation.

## ✨ Features

- Load and validate an employee database from a file
- Add new employee entries
- Delete existing employees by key (e.g. name or address)
- Update employee fields (e.g. working hours)
- Create new databases if they don't exist
- Designed for ease of scripting — minimal interactivity

## 📁 Folder Structure

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
├── logs/ # Valgrind logs  
├── Makefile # Build script  
├── .gitignore  
└── README.md

````

## 🚀 Usage

### Building the project

```bash
make
````
The compiled binary will be located at `./bin/dbview`

### Run the utility

```sh
./bin/roster [options]
```

### Available Options

| Flag         | Description                                    |
| ------------ | ---------------------------------------------- |
| `-f <file>`  | Specify the file path to database file to use  |
| `-n`         | Create a new database file if it doesn't exist |
| `-a <name,address,hours>` | Add a new employee record         |
| `-r <name>`  | Remove an employee by name                     |
| `-u <name,hours>` | Update working hours for an employee by name |
| `-l`         | List employee details                          |

## 💡 Examples
Create a new database
``` bash
./bin/roster -f emp.db -n
```
Add employees
``` bash
./bin/roster -f emp.db -a "Alice Smith,42 Maple St.,180"
./bin/roster -f emp.db -a "Bob Brown,99 Oak Dr.,200"
```
List employees
``` bash
./bin/roster -f emp.db -l
```
Update hours
``` bash
./bin/roster -f emp.db -u "Bob Brown,220"
```
Delete an employee
``` bash
./bin/roster -f emp.db -r "Alice Smith"
```

## 🧪 Testing & Debugging
To test the program with Valgrind (memory analysis):
``` bash
make valgrind
```
Valgrind logs are appended to ./logs/valgrind.log.

## 🤝 Contributing
Pull requests and issues are welcome. For major changes, please open an issue first to discuss what you’d like to change.
