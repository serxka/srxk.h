OUTPUT=test
OBJ=test.o

CFLAGS=-Wall -Wextra -I../
LDFLAGS=-lm

.PHONY: all clean debug release

# Default target is debug
all: debug

debug: CFLAGS += -g -O0 -Ddebug
debug: ${OUTPUT}

release: CFLAGS += -O2 -Drelease
release: ${OUTPUT}

${OUTPUT}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${OUTPUT} $^

# Clean build files
clean:
	rm -f ${OBJ}
	rm -f ${OUTPUT}
