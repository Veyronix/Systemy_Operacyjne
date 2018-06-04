all: program 

program:
	gcc -c server.c
	gcc -o server server.o -L. -lrt
	gcc -c customer.c
	gcc -o customer customer.o -L. -lrt


