#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bstrlib/bstrlib.h"


// headers

typedef struct Object Object;
typedef struct Node Node;

typedef unsigned int (*hasher)(void *obj);
typedef void *(*finder)(Node *self, Object *key);
typedef void (*inserter)(Node *self, Object *key, Object *value);

struct Object {
	hasher hash;
};

struct Node {
	//Object proto;
	finder find;
	inserter insert;
};

typedef struct SingleNode {
	Node proto;
	Object *key;
	Object *value;
} SingleNode;

typedef struct BitmapNode {
	Node Proto;
	Node *values[32];
} BitmapNode;

unsigned int hash(Object *obj, int size) {
	unsigned int hash = 0;
	int c;

	for(c = 0; c < size; c++)
		hash = *((char *) obj + c) + (hash << 6) + (hash << 16) - hash;

	return hash;
}

Node *new_empty_node() {

}

SingleNode *new_single_node() {

}

BitmapNode * new_bitmap_node() {

}

Node *empty_find(Node *self, Object *key) {
	return NULL;
}

void empty_insert(Node *self, Object *key) {

}

Object *single_find(Node *self, Object *key) {
	SingleNode *n = (SingleNode*)self;
	if(n->key->hash(n->key) == key->hash(key)) {
		return n->value;
	} else {
		return NULL;
	}
}

int main(int argc, char *argv[]) {
	return 0;
}
