# Makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = hw4
SRCS = hw4_client.c
HEADERS = hw4_client.h
OBJS = $(SRCS:.c=.o)

# make  
all: $(TARGET)

# compile executable file 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# compile object files 
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# make clean
clean:
	rm -f $(TARGET) $(OBJS)

# make rebuild
rebuild: clean all
