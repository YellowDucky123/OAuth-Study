CC=gcc
CFLAGS=-I.

script: script.o linkedlist.o
	$(CC) -o script script.o linkedlist.o -lssl -lcrypto


script.o: script.c
	$(CC) $(CFLAGS) -c script.c 

linkedlist.o: linkedlist/linkedlist.c
	$(CC) $(CFLAGS) -c linkedlist/linkedlist.c