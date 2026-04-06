# ==========================================
#  Makefile for hashtable demo
# ==========================================

CC      = gcc
CFLAGS  = -O0 -g -Wall -I../include
LDFLAGS = -L.. -lhashtable -Wl,-rpath,$(shell cd .. && pwd)
TARGET  = demo

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
