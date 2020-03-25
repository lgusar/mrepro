#CC = cc
CFLAGS = -Wall -g
LDFLAGS =

BOT_C = bot.c

UDP_SERVER_C = UDP_server.c

all: bot UDP_server

bot: $(BOT_C)
	$(CC) $(BOT_C) -o $@

UDP_server: $(UDP_SERVER_C)
	$(CC) $(UDP_SERVER_C) -o $@
	
clean:
	rm -f bot UDP_server *.o *.core
