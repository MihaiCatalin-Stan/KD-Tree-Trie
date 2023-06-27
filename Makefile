# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -Wpedantic -std=c99 -O0 -g

# define targets
TARGETS=kNN mk

DEPS1=trie.h trie.c
DEPS2=bst.h bst.c
#define object-files
OBJ=trie.o mk.o bst.o kNN.o

build: $(TARGETS)

mk: mk.o $(DEPS1)
	$(CC) $(CFLAGS) $^ -o $@

kNN: kNN.o $(DEPS2)
	$(CC) $(CFLAGS) $^ -o $@ -lm

%.o: %.c $(DEPS1) $(DEPS2)
	$(CC) $(CFLAGS) -c -o $@ $<

pack:
	zip -FSr 315CA_StanMihai-Catalin_Tema3.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJ)

.PHONY: pack clean
