CC = gcc
CFLAGS = -g -Wall -pthread
LDFLAGS = -lrt

TARGET = uthread

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)
