CFLAGS=-Wall -g 

all: hashmap
 
hashmap: hashmap.o object.o

.PHONY: clean all
clean:
	rm *.o hashmap

