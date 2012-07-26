CFLAGS=-Wall -g -O2

all: test
 
test: hashmap.o object.o

.PHONY: clean all
clean:
	rm *.o *.so

