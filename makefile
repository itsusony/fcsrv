fc:
	gcc -Wall -O2 -lpthread -levent -o bin/fcsrv src/main_fcsrv.c
	gcc -Wall -O2 -o bin/client src/client.c
clean:
	rm -f bin/fcsrv
	rm -f bin/client
