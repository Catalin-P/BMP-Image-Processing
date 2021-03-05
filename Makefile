CC=gcc
CFLAGS= -Wall -Wextra -g
RM= rm
run: bmp
	./bmp
clean:
	$(RM) tema3.o bmp
build: tema3.o
	$(CC) $(CFLAGS) tema3.c -o bmp
tema3.o: tema3.c
	$(CC) -c tema3.c