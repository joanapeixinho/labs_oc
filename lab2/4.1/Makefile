TARGET = SimpleCache
PAPILIB_ALAMEDA = /usr/lib/x86_64-linux-gnu/libpapi.so

all: $(TARGET) SimpleProgram

$(TARGET): L1Cache.c
	$(CC) $(CFLAGS) L1Cache.c -o $(TARGET)

SimpleProgram: SimpleProgram.c
	@if [ -e $(PAPILIB_ALAMEDA) ]; then \
		$(CC) $(CFLAGS) SimpleProgram.c $(PAPILIB_ALAMEDA) -o SimpleProgram; \
	else \
		echo "$(PAPILIB_ALAMEDA) does not exist"; \
	fi

clean:
	rm -f $(TARGET)
