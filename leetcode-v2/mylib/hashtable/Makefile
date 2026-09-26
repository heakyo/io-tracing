# ==========================================
#  Makefile for libhashtable.so
# ==========================================

CC      = gcc
CFLAGS  = -O0 -g -Wall -fPIC -Iinclude
TARGET  = libhashtable.so

SRCS    = src/hashtable.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) $(OBJS)
