CCOMP = gcc

LOADER = gcc

OPT = 

client: client_test
	./client_test	
server: server_test
	./server_test	

clinet_test: clinet_test.o socket_utils.h
	$(LOADER) $(OPT) client_test.o socket_utils.h -o client_test

server_test: server_test.o socket_utils.h
	$(LOADER) $(OPT) server_test.o socket_utils.h -o server_test

client_test.o: client_test.c
	$(CCOMP) -c $(OPT) client_test.c

server_test.o: server_test.c
	$(CCOMP) -c $(OPT) server_test.c

clean:
	rm -f *.o server_test client_test