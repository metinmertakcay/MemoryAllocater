all: mm_test install

mm_test: hw3lib.so mm_test.c
	gcc -o mm_test mm_test.c -ldl

hw3lib.so: mm_alloc.o
	gcc -shared -o hw3lib.so mm_alloc.o

mm_alloc.o: mm_alloc.c
	gcc -fpic -c mm_alloc.c

install: hw3lib.so
	cp hw3lib.so /usr/lib/

clean:
	rm /usr/lib/hw3lib.so
