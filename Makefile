CC      = cc
CFLAGS  = -Wall -g
LIBS    = -lpthread

OBJS    = messenger.o client.o server.o get_in_addr.o

messenger: ${OBJS}
	${CC} ${CFLAGS} ${LIBS} -o $@ ${OBJS}

clean:
	-rm *.o messenger

.c.o:
	${CC} ${CFLAGS} -c $<
