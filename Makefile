CFLAGS=-Wall -g -O2

all: test
 
test: hashmap.o object.o

object.o: object.h
hashmap.o: hashmap.h

.PHONY: clean all
clean:
	rm *.o *.so

