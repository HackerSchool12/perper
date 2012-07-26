#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"


//TODO: implement hash and equal for nested maps

void empty_free(Object *self);
void single_free(Object *self);
void bitmap_free(Object *self);
void collision_free(Object *self);

Object *empty_find(BasicNode *self, int level, Object *key);
Object *single_find(BasicNode *self, int level, Object *key);
Object *bitmap_find(BasicNode *self, int level, Object *key);
Object *collision_find(BasicNode *self, int level, Object *key);

BasicNode *empty_insert(BasicNode *self, int level, Object *key, Object *value);
BasicNode *single_insert(BasicNode *self, int level, Object *key, Object *value);
BasicNode *bitmap_insert(BasicNode *self, int level, Object *key, Object *value);
BasicNode *collision_insert(BasicNode *self, int level, Object *key, Object *value);

BasicNode *empty_remove(BasicNode *self, int level, Object *key);
BasicNode *single_remove(BasicNode *self, int level, Object *key);
BasicNode *bitmap_remove(BasicNode *self, int level, Object *key);
BasicNode *collision_remove(BasicNode *self, int level, Object *key);

NodeType empty_type = {NULL, empty_free, empty_find, empty_insert, empty_remove};
NodeType single_type = {NULL, single_free, single_find, single_insert, single_remove};
NodeType bitmap_type = {NULL, bitmap_free, bitmap_find, bitmap_insert, bitmap_remove};
NodeType collision_type = {NULL, collision_free, collision_find, collision_insert, collision_remove};

BasicNode *new_empty_node() {
	static BasicNode *n = NULL;
	if(n == NULL) {
		n = malloc(sizeof(BasicNode));
		n->class = &empty_type;
		n->refcount = 1;
	}
	return n;
}

SingleNode *new_single_node() {
	SingleNode *n = malloc(sizeof(SingleNode));
	n->class = &single_type;
	n->refcount = 1;
	return n;
}

BitmapNode *new_bitmap_node() {
	BitmapNode *n = malloc(sizeof(BitmapNode));
	n->class = &bitmap_type;
	n->refcount = 1;
	int i;
	for(i=0;i<32;i++) {
		n->children[i] = new_empty_node();
	}
	return n;
}

CollisionNode *new_collision_node() {
	CollisionNode *n = malloc(sizeof(CollisionNode));
	n->class = &collision_type;
	n->refcount = 1;
	n->next = NULL;
	return n;
}

void empty_free(Object *self) {
	// singleton, do nothing.
}

void single_free(Object *self) {
	SingleNode *s = (SingleNode*)self;
	release(s->key);
	release(s->value);
	free(s);
}

void bitmap_free(Object *self) {
	BitmapNode *b = (BitmapNode*)self;
	int i;
	for(i=0;i<32;i++) {
		release((Object*)b->children[i]);
	}
	free(b);
}

void collision_free(Object *self) {
	CollisionNode *c = (CollisionNode*)self;
	release((Object*)c->next);
	free(c);
}

Object *empty_find(BasicNode *self, int level, Object *key) {
	return NULL;
}

Object *single_find(BasicNode *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return ((SingleNode*)self)->value;
	} else {
		return NULL;
	}
}

Object *bitmap_find(BasicNode *self, int level, Object *key) {
	BitmapNode *n = (BitmapNode*)self;
	BasicNode *child = n->children[(key->hash >> (5 * level)) & 31];
	return child->class->find(child, (level+1), key);
}

Object *collision_find(BasicNode *self, int level, Object *key) {
	SingleNode *n = (SingleNode*)self;
	CollisionNode *m = (CollisionNode*)self;
	if(n->key->class->equal(n->key, key)) {
		return n->value;
	} else if(m->next != NULL) {
		return collision_find((BasicNode*)m->next, level, key);
	} else {
		return NULL;
	}
}

BasicNode *empty_insert(BasicNode *self, int level, Object *key, Object *value) {
	SingleNode *n = new_single_node();
	retain((Object*)key);
	retain((Object*)value);
	n->key = key;
	n->value = value;
	return (BasicNode*)n;
}

BasicNode *single_insert(BasicNode *self, int level, Object *key, Object *value) {
	SingleNode *node = (SingleNode*)self;

	if(node->key->hash != key->hash) {
		BitmapNode *parent = new_bitmap_node();

		retain((Object*)node);
		parent->children[(node->key->hash >> (5 * level)) & 31] = (BasicNode*)node;

		BasicNode *second_child = parent->children[(key->hash >> (5 * level)) & 31];
		parent->children[(key->hash >> (5 * level)) & 31] = second_child->class->insert(second_child, (level+1), key, value);
		return (BasicNode*)parent;
	} else if (node->key->class->equal(node->key, key)) {
		SingleNode *n = new_single_node();
		retain((Object*)key);
		retain((Object*)value);
		n->key = key;
		n->value = value;
		return (BasicNode*)n;
	} else {
		CollisionNode *col_node = new_collision_node();
		CollisionNode *next_col_node = new_collision_node();

		((SingleNode*)col_node)->key = node->key;
		((SingleNode*)col_node)->value = node->value;
		col_node->next = next_col_node;

		((SingleNode*)next_col_node)->key = key;
		((SingleNode*)next_col_node)->value = value;
		
		return (BasicNode*)col_node;
	}
}

BasicNode *bitmap_insert(BasicNode *self, int level, Object *key, Object *value) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));
	((Object*)new)->refcount = 1;
	
	int i;
	for(i=0;i<32;i++) {
		retain((Object*)node->children[i]);
	}

	BasicNode *child = node->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->class->insert(child, (level+1), key, value);
	
	// we overwrote the incremented one with a new one.
	// reset the old one, we don't ref it.
	release((Object*)child);

	return (BasicNode*)new;
}

BasicNode *collision_insert(BasicNode *self, int level, Object *key, Object *value) {
	CollisionNode *n = new_collision_node();
	retain((Object*)key);
	retain((Object*)value);
	((SingleNode*)n)->key = key;
	((SingleNode*)n)->value = value;
	n->next = (CollisionNode*)self;
	return (BasicNode*)n;
}

BasicNode *empty_remove(BasicNode *self, int level, Object *key) {
	//retain((Object*)self);
	return self;
}

BasicNode *single_remove(BasicNode *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return new_empty_node();
	} else {
		retain((Object*)self);
		return self;
	}
}

BasicNode *bitmap_remove(BasicNode *self, int level, Object *key) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));

	int i;
	for(i=0;i<32;i++) {
		retain((Object*)node->children[i]);
	}
	
	BasicNode *child = new->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->class->remove(child, (level+1), key);

	release((Object*)child);

	return (BasicNode*)new;
}

BasicNode *collision_remove(BasicNode *self, int level, Object *key) {
	SingleNode *n = (SingleNode*)self;
	CollisionNode *m = (CollisionNode*)self;
	if(n->key->class->equal(n->key, key)) {
		if (m->next != NULL) {
			retain((Object*)m->next);
			return (BasicNode*)m->next;
		} else {
			return NULL;
		}
	} else {
		CollisionNode *new = malloc(sizeof(CollisionNode));
		memcpy(new, m, sizeof(CollisionNode));
		new->next = (CollisionNode*)collision_remove((BasicNode*)m->next, level, key);
		return (BasicNode*)new;
	}
}
