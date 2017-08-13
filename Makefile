CC=mpicc
INC_DIR = ./
CFLAGS=-g -Wall -Wstrict-prototypes -c -I$(INC_DIR)
LDFLAGS=-o 

all: make_graph print_graph floyd_serial floyd_parallel 

floyd_parallel:	floyd_parallel.o MyMPI.o graph.o
	${CC} ${LDFLAGS} floyd_parallel floyd_parallel.o MyMPI.o graph.o

floyd_parallel.o:	floyd_parallel.c MyMPI.h graph.h
	$(CC) ${CFLAGS} floyd_parallel.c -I${INC_DIR} 

floyd_serial:	floyd_serial.o graph.o
	${CC} ${LDFLAGS} floyd_serial floyd_serial.o graph.o

floyd_serial.o:	floyd_serial.c graph.h
	$(CC) ${CFLAGS} floyd_serial.c -I${INC_DIR}

print_graph:	print_graph.o
	${CC} ${LDFLAGS} print_graph print_graph.o

print_graph.o:	print_graph.c
	$(CC) ${CFLAGS} print_graph.c

make_graph:	make_graph.o graph.o
	${CC} ${LDFLAGS} make_graph make_graph.o graph.o

make_graph.o:	make_graph.c graph.h
	$(CC) ${CFLAGS} make_graph.c -I${INC_DIR}

graph.o:	graph.c graph.h 
	$(CC) ${CFLAGS} graph.c 

MyMPI.o:	MyMPI.c MyMPI.h
	$(CC) ${CFLAGS} MyMPI.c 

clean:
	rm -rf *.o
	rm -rf *~
	rm -rf make_graph print_graph floyd_serial floyd_parallel
