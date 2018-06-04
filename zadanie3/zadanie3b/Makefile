all: program clean

program:
	gcc -c main.c
	gcc -o main main.o
	gcc -c failProgram.c
	gcc -o failProgram failProgram.o
	./main test 10 100

clean:
	rm -f *.o
