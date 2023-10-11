CC = gcc
CFLAGS = -Wall -Wextra
TARGET = SimpleProgram
PAPILIB_ALAMEDA = /usr/lib/x86_64-linux-gnu/libpapi.so

SRCS = SimpleProgram.c L1Cache.c

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(PAPILIB_ALAMEDA) -o $(TARGET)

all: $(TARGET)

clean:
	rm $(TARGET)
