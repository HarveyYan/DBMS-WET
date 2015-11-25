all: wet

wet:  wet.h wet.c parser.c
	gcc -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq -o wet wet.c parser.c

clean:
	rm -f wet