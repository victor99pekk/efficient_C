
CC = gcc
CFLAGS = -lm

all: intopt
	./a.out < i

intopt: intopt.c
	$(CC) -g intopt.c $(CFLAGS)

clean: 
	rm hello

sanitizer:
	$(CC) -g -fsanitize=address intopt.c $(CFLAGS)
	./a.out < i