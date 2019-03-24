CFLAGS=-std=c99 -Wall -Wextra -pedantic

all : extrakb fselect

fselect : fselect.o
extrakb : extrakb.o

%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean :
	rm -rf *.o extrakb fselect
