CC = g++
CFLAGS = -g

default: server

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp

clean:
	rm -f server
