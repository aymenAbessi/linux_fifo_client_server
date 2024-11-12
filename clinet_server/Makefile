CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lrt

# List of source files
SRCS = server.c client.c launcher.c
# Corresponding object files
OBJS = server.o client.o launcher.o

# Target for building both server, client, and launcher
all: server client launcher

# Rule to build the server executable
server: server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build the client executable
client: client.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Rule to build the launcher executable
launcher: launcher.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# General rule to compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to run the server
run_server:
	./server

# Rule to run the client
run_client:
	./client

# Rule to run the launcher
run_launcher:
	./launcher

# Rule to clean up generated files
clean:
	rm -f $(OBJS) server client launcher

.PHONY: all clean run_server run_client run_launcher
