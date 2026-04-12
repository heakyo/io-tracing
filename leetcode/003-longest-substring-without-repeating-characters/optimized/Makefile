# ==========================================
#  Makefile for 003-longest-substring (Optimized)
# ==========================================

# 1. Compiler and Flags
CC      = gcc
CFLAGS  = -O0 -g
TARGET  = main

# 2. Source and Object Files
SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)

# 3. Default Rule
all: $(TARGET)

# 4. Linking Stage
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 5. Compilation Stage
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 6. Cleanup Rule
.PHONY: clean
clean:
	@echo "Cleaning up project..."
	rm -f $(TARGET) $(OBJS)
