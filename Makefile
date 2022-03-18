CC     = cc
BIN    = collect
SRC    = collect.c
OBJ    = $(SRC:.c=.o)
INC    = config.h
PREFIX = /usr/local
CFLAGS = -O1 -Wall -g

.PHONY: all clean install
all: $(BIN)

$(BIN): $(OBJ) $(INC)
	$(CC) -o $@ $^ $(CFLAGS)

.SUFFIXES: .c .o
.c.o:
	$(CC) -c -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) $(BIN)

install:
	strip $(BIN)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin
