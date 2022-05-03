include config.mk

BIN    = collect
SRC    = collect.c
OBJ    = $(SRC:.c=.o)
INC    = config.h
PREFIX = /usr/local

.PHONY: all clean install
all: $(BIN)

$(BIN): $(OBJ) $(INC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

clean:
	rm -f $(OBJ) $(BIN)

install: $(BIN)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BIN) $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/doc/collect
	install -m 644 COPYING $(DESTDIR)$(PREFIX)/share/doc/collect
	install -m 644 README $(DESTDIR)$(PREFIX)/share/doc/collect
	install -d $(DESTDIR)$(MANPREFIX)
	install -m 644 collect.1 $(DESTDIR)$(PREFIX)/share/man/man1
	gzip -f $(DESTDIR)$(PREFIX)/share/man/man1/collect.1
