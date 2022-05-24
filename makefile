CC = gcc
TARGET = demo
FLAGS = -02 

all: demo.o RedBlackTree.o
	$(CC) -o $(TARGET) $^

clean:
	rm -f *.o
	rm -f $(TARGET)

demo.o: demo.c RBT.h
	$(CC) $(CFLAGS) -c demo.c -o demo.o


RedBlackTree.o: RedBlackTree.c RBT.h
	$(CC) $(CFLAGS) -c RedBlackTree.c -o RedBlackTree.o

