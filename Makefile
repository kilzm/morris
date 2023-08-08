CFLAGS=-std=gnu11 -g -O3 -pthread
WFLAGS=-Wall -Wextra -Wpedantic -Wshadow -Wfloat-equal -Wpointer-arith -Wcast-align 	   \
	-Wstrict-prototypes -Wno-strict-overflow -Wwrite-strings -Waggregate-return -Wcast-qual \
	-Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
COUT=-o $@

SRCD=./src
BIND=./bin

SOURCES=$(shell find ./src -name "*.c")
TARGET=sysprak-client

.PHONY: all
all: $(BIND)/$(TARGET)

$(BIND)/$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(WFLAGS) $(COUT) $^


.PHONY: clean
clean:
	rm -f *.o $(BIND)/$(TARGET)
