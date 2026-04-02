# ==========================================
#  Makefile for Hello World Project
# ==========================================

# 1. Compiler and Flags
CC      = gcc
CFLAGS  = -Wall -Wextra -O0 -g
TARGET  = main

# 2. Source and Object Files
# Collect all .c files in the current directory
SRCS    = $(wildcard *.c)
# Convert the list of .c files into .o files
OBJS    = $(SRCS:.c=.o)

# 3. Default Rule (The first rule is executed by default)
all: $(TARGET)

# 4. Linking Stage
# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 5. Compilation Stage
# Compile each .c file into a .o object file
# $< : the name of the first prerequisite (the .c file)
# $@ : the name of the target (the .o file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 6. Cleanup Rule
# .PHONY ensures that 'make clean' works even if a file named 'clean' exists
.PHONY: clean
clean:
	@echo "Cleaning up project..."
	rm -f $(TARGET) $(OBJS)
