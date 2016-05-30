C = gcc
FLAGS = -Wall

# Creating a run file for generalSwap
generalSwap: generalSwap.c
	$(C) $(FLAGS) generalSwap.c -o generalSwap

# Compiling GenRangeTree
GenRangeTree.o: GenRangeTree.c GenRangeTree.h
	$(C) $(FLAGS) -D NDEBUG=1 -c GenRangeTree.c -o GenRangeTree.o

# Creating from GenRangeTree.o libGenRangeTree.a
GenRangeTree: GenRangeTree.o
	ar rcs libGenRangeTree.a GenRangeTree.o

# Running the unit testing
tests: GenRangeTree.c GenRangeTree.h
	$(C) $(FLAGS) GenRangeTree.c -o debugTreeFile

# Running valgrind check
valgrind: GenRangeTree
	valgrind --log-file="valdbg.out" GenRangeTree

# Creating a tar file
tar: generalSwap.c GenRangeTree.c GenRangeTree.h Makefile
	tar cvf ex3.tar generalSwap.c GenRangeTree.c GenRangeTree.h valdbg.out Makefile

clean: 
	rm -f generalSwap debugTreeFile libGenRangeTree.a ex3 valdbg.out
	rm -f *.o

.PHONY: GenRangeTree tests tar clean debugTreeFile