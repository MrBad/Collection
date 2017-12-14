CC=gcc
INCLUDE=-I.
DFLAGS=-DDO_TESTS
LIBS=
OFLAGS=-c
CFLAGS=-g -Wall -Wextra -std=c99 -pedantic-errors $(INCLUDE) $(DFLAGS)

OBJECTS=collection.o main.o
TARGET=collection

all: $(OBJECTS) Makefile
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

%.o: %.c Makefile *.h
	$(CC) $(CFLAGS) $(OFLAGS) -o $@ $<
run:
	./$(TARGET)
clean:
	rm $(OBJECTS) $(TARGET) core
