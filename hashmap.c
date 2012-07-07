#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

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
	n->count = 0;
	int i;
	for(i=0;i<32;i++) {
		n->children[i] = new_empty_node();
	}
	return n;
}

CollisionNode * new_collision_node() {
	CollisionNode *n = malloc(sizeof(CollisionNode));
	n->next = NULL;
	return n;
}

Object *empty_find(Node *self, int level, hash_t hash, Object *key) {
	return NULL;
}

Object *single_find(Node *self, int level, hash_t hash, Object *key) {
	if(((SingleNode*)self)->hash == hash) {
		return ((SingleNode*)self)->value;
	} else {
		return NULL;
	}
}

Object *bitmap_find(Node *self, int level, hash_t hash, Object *key) {
	BitmapNode *n = (BitmapNode*)self;
	Node *child = n->children[(hash >> (5 * level)) & 31];
	return child->find(child, level++, hash, key);
}

Object *collision_find(Node *self, int level, hash_t hash, Object *key);

Node *empty_insert(Node *self, int level, hash_t hash, Object *key, Object *value) {
	SingleNode *n = new_single_node();
	n->key = key;
	n->hash = hash;
	n->value = value;
	return (Node*)n;
}

Node *single_insert(Node *self, int level, hash_t hash, Object *key, Object *value) {
	SingleNode *node = (SingleNode*)self;

	if(node->hash == hash) {
		CollisionNode *col_node = new_collision_node();
		CollisionNode *next_col_node = new_collision_node();

		col_node->proto = *node;
		col_node->next = next_col_node;

		next_col_node->proto.key = key;
		next_col_node->proto.hash = hash;
		next_col_node->proto.value = value;
		
		return (Node*)col_node;
	} else {
		BitmapNode *parent = new_bitmap_node();

		parent->children[(node->hash >> (5 * level)) & 31] = (Node*)node;

		Node *second_child = parent->children[(hash >> (5 * level)) & 31];
		parent->children[(hash >> (5 * level)) & 31] = second_child->insert(second_child, level++, hash, key, value);

		return (Node*)parent;
	}
}

Node *bitmap_insert(Node *self, int level, hash_t hash, Object *key, Object *value) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));

	Node *child = new->children[(hash >> (5 * level)) & 31];
	new->children[(hash >> (5 * level)) & 31] = child->insert(child, level++, hash, key, value);

	return (Node*)new;
}

Node *collision_insert(Node *self, int level, hash_t hash, Object *key, Object *value);

Node *empty_remove(Node *self, int level, hash_t hash, Object *key) {
	return self;
}

Node *single_remove(Node *self, int level, hash_t hash, Object *key) {
	if(((SingleNode*)self)->hash == hash) {
		return new_empty_node();
	} else {
		return self;
	}
}

Node *bitmap_remove(Node *self, int level, hash_t hash, Object *key) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));
	
	Node *child = new->children[(hash >> (5 * level)) & 31];
	new->children[(hash >> (5 * level)) & 31] = child->remove(child, level++, hash, key);

	return (Node*)new;
}

Node *collision_remove(Node *self, int level, hash_t hash, Object *key);

int main(int argc, char **argv) {
	printf("hello world\n");
	Node *myhash = new_empty_node();
	OString *key = new_ostring("fooo baar");
	OString *value = new_ostring("baz boo");

	Node *newhash = myhash->insert(myhash, 0, key->proto.hash((Object*)key), (Object*)key, (Object*)value);

	printf("the value is %s\n", OSTR2CSTR(newhash->find(newhash, 0, key->proto.hash((Object*)key), (Object*)key)));

	key = new_ostring("blaha");
	value = new_ostring("jhshjda");

	Node *newnewhash = myhash->insert(myhash, 0, key->proto.hash((Object*)key), (Object*)key, (Object*)value);

	printf("the value is %s\n", OSTR2CSTR(newnewhash->find(newnewhash, 0, key->proto.hash((Object*)key), (Object*)key)));

	key = new_ostring("kkjs");
	value = new_ostring("fooo");

	Node *newnewnewhash = myhash->insert(myhash, 0, key->proto.hash((Object*)key), (Object*)key, (Object*)value);

	printf("the value is %s\n", OSTR2CSTR(newnewnewhash->find(newnewnewhash, 0, key->proto.hash((Object*)key), (Object*)key)));

}
