CC				= gcc
CFLAGS			= -O4 -Wall -I/usr/local/include -Wno-missing-braces -Wno-unused-result
DEST			= /usr/local/bin
LDFLAGS			= -L/usr/local/lib
LIBS			= 
OBJS			= asm6.o
PROGRAM			= asm6

all:			$(PROGRAM)

$(PROGRAM):		$(OBJS)
				$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:
				rm -f *.o *~ $(PROGRAM)

install:		$(PROGRAM)
				install -s $(PROGRAM) $(DEST)