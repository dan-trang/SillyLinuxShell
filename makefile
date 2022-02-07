CC = gcc
CFLAGS = -g -Wall
TARGET = silly

all:
	$(CC) $(CFLAGS) sillyshell.c -o $(TARGET)

clean:
	$(RM) silly *.o *~
