CC=g++
DEV=-Wall -g -std=c++14
OPT=-O3 -std=c++14

#AVLCOMMANDS_SRC=AVLCommands.cpp
#AVLCOMMANDS_HDR=$(AVLCOMMANDS_SRC:.cpp=.h)
#AVLCOMMANDS_OBJ=$(AVLCOMMANDS_SRC:.cpp=.o)

.PHONY: all
all: BSTSanityCheck AVLCommands CreateData

CreateData: CreateData.cxx json.hpp
	$(CC) $(OPT) CreateData.cxx -o CreateData

#EXECUTABLE
AVLCommands: AVLCommands.o json.hpp 
	$(CC) $(DEV) -o AVLCommands AVLCommands.o

BSTSanityCheck: BSTSanityCheck.cxx BST.o 
	$(CC) $(DEV) BSTSanityCheck.cxx BST.o -o BSTSanityCheck

#OBJECT 
BST.o: BST.cpp BST.h
	$(CC) $(DEV) -c BST.cpp

AVLCommands.o: AVlCommands.cpp AVLCommands.h 
	$(CC) $(DEV) -c AVLCommands.cpp


# Build
.PHONY: clean
clean:
	rm -f *.o
	rm -f *.exe
	rm -rf *dSYM

.PHONY: update
update:
	make clean
	make all
