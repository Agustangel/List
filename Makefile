
INCLUDES=include
SOURCE=source

list: main.o list.o
	gcc -o list $^

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

list.o: ${SOURCE}/list.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

valgrind: list
	valgrind --leak-check=yes ./list

clean:
	rm list main.o list.o
	