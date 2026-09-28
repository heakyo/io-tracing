# ==========================================
#  Makefile for hashtable demo
# ==========================================

CC      = gcc
CFLAGS  = -O0 -g -Wall -I../include
CFLAGS  += -fsanitize=address,undefined
LDFLAGS = -L.. -lhashtable -Wl,-rpath,$(shell cd .. && pwd)
LDFLAGS += -fsanitize=address,undefined
TARGET  = main

SRCS    = main.c
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) $(OBJS)
