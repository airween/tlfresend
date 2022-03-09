
CFLAGS = $(shell pkg-config --cflags glib-2.0)
LDLIBS = $(shell pkg-config --libs glib-2.0)

all:
	gcc -Wall $(CFLAGS) main.c utils.c getpx.c \
	-o tlftest $(LDLIBS)

clean:
	rm -rf *.o tlftest
