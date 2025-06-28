TARGET = bin/dbview

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET)

# Default target
default: $(TARGET)

# Clean target
clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

# Link object files into final binary
$(TARGET): $(OBJ)
	gcc -o $@ $?

# Compile source files into object files
obj/%.o : src/%.c
	gcc -c $< -o $@ -Iinclude

# Initialize and run standard test sequence
test: clean default init populate validate update remove validate

init:
	@echo "\n===== Initializing database =====\n"
	@./$(TARGET) -f ./emp.db -n

populate:
	@echo "\n===== Populating database =====\n"
	@./$(TARGET) -f ./emp.db -a "John Doe,96 Shire Ln., 230"
	@./$(TARGET) -f ./emp.db -a "Jane Doe,96 Shire Ln., 200"
	@./$(TARGET) -f ./emp.db -a "Jack Doe,96 Shire Ln., 190"

validate:
	@echo "\n===== Listing employees: =====\n"
	@./$(TARGET) -f ./emp.db -l

update:
	@echo "\n===== Update hours =====\n"
	@./$(TARGET) -f ./emp.db -u "Jane Doe, 220"

remove:
	@echo "\n===== Remove employees =====\n"
	@./$(TARGET) -f ./emp.db -r "Jack Doe"
	@./$(TARGET) -f ./emp.db -l

log: | logs
	@echo "\n===== Valgrind run at $$(date) =====\n" >> ./logs/valgrind.log
	@valgrind --leak-check=full --show-leak-kinds=all ./bin/dbview -s -f ./emp.db -l >> ./logs/valgrind.log 2>&1

logs:
	mkdir -p logs
