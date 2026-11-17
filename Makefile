# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

# All source files
SRCS = src/main.c \
       src/file_manager.c \
       src/file_operations.c \
       src/directory_operations.c \
       src/storage.c

# Derive object file names from source file names
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = MiniFileSystem

# Default rule: build the program
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Pattern rule: compile any .c into its matching .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Remove compiled files
clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean
