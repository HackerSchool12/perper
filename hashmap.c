#include <stdlib.h>
#include <stdio.h>

#include "hashmap.h"

unsigned int hash(Object *obj, int size) {
	unsigned int hash = 0;
	int c;

	for(c = 0; c < size; c++)
		hash = *((char *) obj + c) + (hash << 6) + (hash << 16) - hash;

	return hash;
}

Node *new_empty_node() {
	static Node *n = NULL;
	if(n == NULL) {
		n = malloc(sizeof(Node));
		n->find = empty_find;
		n->insert = empty_insert;
		n->remove = empty_remove;
	}
	return n;
}

SingleNode *new_single_node() {
	SingleNode *n = malloc(sizeof(SingleNode));
	n->proto.find = single_find;
	n->proto.insert = single_insert;
	n->proto.remove = single_remove;
	return n;
}

BitmapNode * new_bitmap_node() {
	BitmapNode *n = malloc(sizeof(BitmapNode));
	n->proto.find = bitmap_find;
	n->proto.insert = bitmap_insert;
	n->proto.remove = bitmap_remove;
	return n;
}

CollisionNode * new_collision_node();

Object *empty_find(Node *self, unsigned int hash, Object *key) {
	return NULL;
}

Object *single_find(Node *self, unsigned int hash, Object *key) {
	if(((SingleNode*)self)->hash == hash) {
		return ((SingleNode*)self)->value;
	} else {
		return NULL;
	}
}

Object *bitmap_find(Node *self, unsigned int hash, Object *key);
Object *collision_find(Node *self, unsigned int hash, Object *key);

Node *empty_insert(Node *self, unsigned int hash, Object *key, Object *value) {
	SingleNode *n = new_single_node();
	n->key = key;
	n->hash = hash;
	n->value = value;
	return (Node*)n;
}

Node *single_insert(Node *self, unsigned int hash, Object *key, Object *value);
Node *bitmap_insert(Node *self, unsigned int hash, Object *key, Object *value);
Node *collision_insert(Node *self, unsigned int hash, Object *key, Object *value);

Node *empty_remove(Node *self, unsigned int hash, Object *key) {
	return self;
}

Node *single_remove(Node *self, unsigned int hash, Object *key) {
	if(((SingleNode*)self)->hash == hash) {
		return new_empty_node();
	} else {
		return self;
	}
}
Node bitmap_remove(Node *self, unsigned int hash, Object *key);
Node collision_remove(Node *self, unsigned int hash, Object *key);
