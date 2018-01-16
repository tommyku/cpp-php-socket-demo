CC = g++
CFLAGS = -g

default: server

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp

server_sc: server_single_connect.cpp
	$(CC) $(CFLAGS) -o server_sc server_single_connect.cpp

clean:
	rm -f server server_sc
