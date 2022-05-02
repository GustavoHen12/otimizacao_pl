CFLAGS = -Wall
LDFLAGS = -lm -ldl
DEBUG_FLAGS = -g -DDEBUG
LP_SOLVE_FLAGS = liblpsolve55.so
COMPILER = -std=gnu99

all: envio

envio: envio.c data_strucs.o
	gcc -o envio envio.c data_strucs.o $(CFLAGS) $(LDFLAGS)

data_strucs.o: data_strucs.c data_strucs.h
	$(CC) -c data_strucs.c -o data_strucs.o

parcial-relaxada: parcial-relaxada.c
	gcc -o parcial-relaxada parcial-relaxada.c $(CFLAGS) $(LDFLAGS) $(LP_SOLVE_FLAGS)

parcial-relaxada-debug: parcial-relaxada.c
	gcc -o parcial-relaxada parcial-relaxada.c $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) $(LP_SOLVE_FLAGS)

clean:
	-rm -f *~ *.o
	
purge: clean
	-rm -f envio
