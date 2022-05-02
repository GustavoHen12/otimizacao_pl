DEBUG= -DDEBUG
CC= gcc

all: p2

debug: p2_functions.c data_strucs.o
	$(CC) p2_functions.c data_strucs.o -o p2 $(DEBUG)

p2: p2_functions.c data_strucs.o
	$(CC) p2_functions.c data_strucs.o -o p2

queue.o: data_strucs.c data_strucs.h
	$(CC) -c data_strucs.c -o data_strucs.o

clean:
	rm *.o

purge: clean
	rm p2
