CC = gcc
CFLAGS = -Wall -pthread `mysql_config --cflags`
LDFLAGS = `mysql_config --libs`

all: server client

server: server.c protocol.o database.o utils.o
	$(CC) $(CFLAGS) -o server server.c protocol.o database.o utils.o $(LDFLAGS)

client: client.c menu.o utils.o display.o
	$(CC) $(CFLAGS) -o client client.c menu.o utils.o display.o $(LDFLAGS)

protocol.o: protocol.c
	$(CC) $(CFLAGS) -c protocol.c

database.o: database.c
	$(CC) $(CFLAGS) -c database.c

menu.o: menu.c
	$(CC) $(CFLAGS) -c menu.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

display.o: display.c
	$(CC) $(CFLAGS) -c display.c

clean:
	rm -f *.o server client
