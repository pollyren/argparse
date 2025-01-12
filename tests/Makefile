CC = gcc
CFLAGS = -Wall -Wextra -I.. -lcriterion -Wimplicit-fallthrough
SRC = ../argparse.c test_argparse.c
TARGET = test_argparse

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
