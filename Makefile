CFLAGS = -Wall
LDFLAGS = -lm -ldl
DEBUG_FLAGS = -g -DDEBUG
LP_SOLVE_FLAGS = liblpsolve55.so
COMPILER = -std=gnu99

all: parcial-relaxada

parcial-relaxada: parcial-relaxada.c
	gcc -o parcial-relaxada parcial-relaxada.c $(CFLAGS) $(LDFLAGS) $(LP_SOLVE_FLAGS)

parcial-relaxada-debug: parcial-relaxada.c
	gcc -o parcial-relaxada parcial-relaxada.c $(CFLAGS) $(LDFLAGS) $(DEBUG_FLAGS) $(LP_SOLVE_FLAGS)

clean:
	-rm -f *~ *.o
	
purge: clean
	-rm -f parcial-relaxada
