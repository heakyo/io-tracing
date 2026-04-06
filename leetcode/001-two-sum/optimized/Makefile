# ==========================================
#  Makefile for 001-two-sum optimized
#  Links against libhashtable.so from mylib
# ==========================================

CC      = gcc
CFLAGS  = -O0 -g -I../../mylib/hashtable/include
LDFLAGS = -L../../mylib/hashtable -lhashtable \
          -Wl,-rpath,$(shell cd ../../mylib/hashtable && pwd)
TARGET  = main

SRCS    = $(wildcard *.c)
OBJS    = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning up project..."
	rm -f $(TARGET) $(OBJS)
