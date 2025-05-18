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
