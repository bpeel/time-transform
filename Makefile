OBJS = main.o matrix-functions.o
FLAGS_COMMON=-g -Wall -O3
CFLAGS = `pkg-config glib-2.0 --cflags` $(FLAGS_COMMON)
LDFLAGS = `pkg-config glib-2.0 --libs`
CFLAGS32 = `PKG_CONFIG_PATH=/usr/lib/pkgconfig pkg-config glib-2.0 --cflags` -msse2 -mfpmath=sse $(FLAGS_COMMON) -m32
LDFLAGS32 = `PKG_CONFIG_PATH=/usr/lib/pkgconfig pkg-config glib-2.0 --libs` -m32

all : time-transform time-transform-32

time-transform : $(OBJS)
	gcc -o $@ $(LDFLAGS) $(OBJS)

%.o : %.c
	gcc -c $(CFLAGS) -o $@ $<

time-transform-32 : $(OBJS:.o=.32.o)
	gcc -o $@ $(LDFLAGS32) $(OBJS:.o=.32.o)

%.32.o : %.c
	gcc -c $(CFLAGS32) -o $@ $<

main.o : header.h
main.32.o : header.h

matrix-functions.o : header.h
matrix-functions.32.o : header.h

clean :
	rm -f *.o time-transform time-transform-32

.PHONY : clean all
