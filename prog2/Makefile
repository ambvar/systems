CC = gcc
OBJ_FILES = *.o
OPTS = -std=c89 -g -pedantic -Wall -I.
EXE = dealer player

all: dealer player

dealer: dealer.c
	$(CC) $(OPTS) -o dealer dealer.c

player: player.c
	$(CC) $(OPTS) -o player player.c

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ_FILES)
