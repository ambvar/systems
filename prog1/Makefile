CC = gcc
SRC_FILES = *.c
OBJ_FILES = *.o
OPTS = -g -pedantic -Wall -I.
EXE = customsort

build: $(SRC_FILES)
	$(CC) $(OPTS) -o $(EXE) $(SRC_FILES)

.PHONY: clean

clean:
	rm -f $(EXE) $(OBJ_FILES)
