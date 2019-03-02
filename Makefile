CC = g++
CFLAGS = -Iinc -Wall -Werror -pedantic-errors -Wextra -std=c++17 -ftrapv -O2 -g
OBJECTS1 = bin/g512sum.o bin/functions.o bin/mappings.o bin/hash.o
OBJECTS2 = bin/g256sum.o bin/functions.o bin/mappings.o bin/hash.o
EXEC1 = g512sum.out
EXEC2 = g256sum.out
BLOCK = inc/block.hpp inc/block.cpp

all : $(EXEC1) $(EXEC2)

$(EXEC1) : $(OBJECTS1)
	$(CC) $(OBJECTS1) -o $(EXEC1)

$(EXEC2) : $(OBJECTS2)
	$(CC) $(OBJECTS2) -o $(EXEC2)

bin/g512sum.o : src/g512sum.cpp inc/functions.hpp inc/hash.hpp $(BLOCK)
	$(CC) $< $(CFLAGS) -c -o $@

bin/g256sum.o : src/g256sum.cpp inc/functions.hpp inc/hash.hpp $(BLOCK)
	$(CC) $< $(CFLAGS) -c -o $@

bin/functions.o : src/functions.cpp inc/functions.hpp $(BLOCK)
	$(CC) $< $(CFLAGS) -c -o $@

bin/mappings.o : src/mappings.cpp inc/hash.hpp $(BLOCK)
	$(CC) $< $(CFLAGS) -c -o $@

bin/hash.o : src/hash.cpp inc/hash.hpp $(BLOCK)
	$(CC) $< $(CFLAGS) -c -o $@

clean :
	rm -f *.out bin/*.o *.txt
	rm -rf *.dSYM
