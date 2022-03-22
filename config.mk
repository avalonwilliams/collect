PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CC = cc

CPPFLAGS = -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -O1 -std=c99 -Wall -pedantic
