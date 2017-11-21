CC	=/usr/bin/g++
CFLAGS	=-Wall -fopenmp
BIN	=Openmtree

OBJ	=Openmtree.o

Tree:	$(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

%.o:	%.cpp
	$(CC) $(CFLAGS) -c $<

.PHONY:	clean
clean:
	rm -rf $(BIN) $(OBJ)
