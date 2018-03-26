all: program clean

program:
	gcc -c main.c
	gcc -o main main.o
	./main test 100 100

clean:
	rm -f *.o
