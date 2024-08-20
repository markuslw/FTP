CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

SERVER_SRCS = server/server.c server/ipaddr.c
CLIENT_SRCS = client/client.c

SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

SERVER_EXEC = server/server
CLIENT_EXEC = client/client

.PHONY: all clean cleanall

all: $(SERVER_EXEC) $(CLIENT_EXEC) clean

$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS)

cleanall:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_EXEC) $(CLIENT_EXEC)
