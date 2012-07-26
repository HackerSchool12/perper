CFLAGS=-Wall -g

all: test
 
test: hashmap.o object.o

.PHONY: clean all
clean:
	rm *.o *.so

