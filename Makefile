OBJS = main.o matrix-functions.o
CFLAGS = `pkg-config glib-2.0 --cflags` -g -Wall -msse2 -mfpmath=sse -m32 -O3
LDFLAGS = `pkg-config glib-2.0 --libs` -m32

all : time-transform

time-transform : $(OBJS)
	gcc -o $@ $(LDFLAGS) $(OBJS)

.c.o :
	gcc -c $(CFLAGS) -o $@ $<

main.o : header.h

matrix-functions.o : header.h

clean :
	rm -f $(OBJS) time-transform

.PHONY : clean all
