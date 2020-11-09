include config.mk

NAME=sman
SRC=$(wildcard src/**/*.c)
SRC+=$(wildcard src/*.c)
CFLAGS?=-O2 -s -Wall
CFLAGS+=-I. -Isrc

DEP=

DEP+=lib/argparse/argparse.c
CFLAGS+=-Ilib/argparse

DEP+=lib/inih/ini.c
CFLAGS+=-Ilib/inih

OBJ=$(SRC:.c=.o)
OBJ+=$(DEP:.c=.o)

.PHONY: all
all: $(NAME)

lib/argparse/argparse.c:
	mkdir -p lib/argparse
	curl -sL https://github.com/cofyc/argparse/archive/master.tar.gz > lib/argparse/source.tar.gz
	tar xzf lib/argparse/source.tar.gz -C lib/argparse --strip-components=1

lib/inih/ini.c:
	mkdir -p lib/inih
	curl -sL https://github.com/benhoyt/inih/archive/master.tar.gz > lib/inih/source.tar.gz
	tar xzf lib/inih/source.tar.gz -C lib/inih --strip-components=1

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(LIBS) $(OBJ) -o $@

.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f $(NAME) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < man/$(NAME).1 > $(DESTDIR)$(MANPREFIX)/man1/$(name).1

.PHONY: clean
clean:
	rm -rf lib
	rm -f $(OBJ)
