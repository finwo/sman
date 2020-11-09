NAME=sman
SRC=$(wildcard src/*.c)
LIBS:=
INCLUDES?=
CFLAGS?=-O2 -s -Wall
INCLUDES+=-I. -Isrc

LIBS+=lib/argparse
SRC+=lib/argparse/argparse.c
INCLUDES+=-Ilib/argparse

LIBS+=lib/inih
SRC+=lib/inih/ini.c
INCLUDES+=-Ilib/inih

OBJ=$(SRC:.c=.o)

default: $(NAME)

lib/argparse:
	mkdir -p $(@)
	curl -sL https://github.com/cofyc/argparse/archive/master.tar.gz > $(@)/source.tar.gz
	tar xzvf $(@)/source.tar.gz -C $(@) --strip-components=1

lib/inih:
	mkdir -p $(@)
	curl -sL https://github.com/benhoyt/inih/archive/master.tar.gz > $(@)/source.tar.gz
	tar xzvf $(@)/source.tar.gz -C $(@) --strip-components=1

.c.o: $(@:.c=.o)
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(NAME): $(LIBS) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

.PHONY: clean
clean:
	rm -rf $(LIBS)
	rm -f $(OBJ)
