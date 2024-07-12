CC = gcc
CFLAGS = -O2 -Wall

DEMO_SRC = ./demo.c ./rbtree.c

all: demo

demo: $(DEMO_SRC)
	$(CC) $(CFLAGS) -o demo $(DEMO_SRC) 

clean:
	rm -f demo