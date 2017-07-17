todos: cliente.x servidor.x clear

cliente.o: cliente.c
	gcc -c cliente.c

servidor.o: servidor.c
	gcc -c servidor.c

cliente.x: cliente.o
	gcc -o cliente.x cliente.o

servidor.x: servidor.o
	gcc -o servidor.x servidor.o
	
clear: 
	rm *.o