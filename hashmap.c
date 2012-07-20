#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

Node *new_empty_node() {
	static Node *n = NULL;
	if(n == NULL) {
		n = malloc(sizeof(Node));
		((Object*)n)->class = EMPTYNODE;
		n->find = empty_find;
		n->insert = empty_insert;
		n->remove = empty_remove;
		((Object*)n)->free = empty_free;
	}
	return n;
}

SingleNode *new_single_node() {
	SingleNode *n = malloc(sizeof(SingleNode));
	((Object*)n)->class = SINGLENODE;
	((Node*)n)->find = single_find;
	((Node*)n)->insert = single_insert;
	((Node*)n)->remove = single_remove;
	((Object*)n)->refcount = 1;
	((Object*)n)->free = single_free;
	return n;
}

BitmapNode * new_bitmap_node() {
	BitmapNode *n = malloc(sizeof(BitmapNode));
	((Object*)n)->class = BITMAPNODE;
	((Node*)n)->find = bitmap_find;
	((Node*)n)->insert = bitmap_insert;
	((Node*)n)->remove = bitmap_remove;
	((Object*)n)->refcount = 1;
	((Object*)n)->free = bitmap_free;
	int i;
	for(i=0;i<32;i++) {
		n->children[i] = new_empty_node();
	}
	return n;
}

CollisionNode * new_collision_node() {
	CollisionNode *n = malloc(sizeof(CollisionNode));
	((Object*)n)->class = COLLISIONNODE;
	((Node*)n)->find = collision_find;
	((Node*)n)->insert = collision_insert;
	((Node*)n)->remove = collision_remove;
	((Object*)n)->refcount = 1;
	((Object*)n)->free = collision_free;
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

Object *empty_find(Node *self, int level, Object *key) {
	return NULL;
}

Object *single_find(Node *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return ((SingleNode*)self)->value;
	} else {
		return NULL;
	}
}

Object *bitmap_find(Node *self, int level, Object *key) {
	BitmapNode *n = (BitmapNode*)self;
	Node *child = n->children[(key->hash >> (5 * level)) & 31];
	return child->find(child, (level+1), key);
}

Object *collision_find(Node *self, int level, Object *key) {
	SingleNode *n = (SingleNode*)self;
	CollisionNode *m = (CollisionNode*)self;
	if(n->key->equal(n->key, key)) {
		return n->value;
	} else if(m->next != NULL) {
		return collision_find((Node*)m->next, level, key);
	} else {
		return NULL;
	}
}

Node *empty_insert(Node *self, int level, Object *key, Object *value) {
	SingleNode *n = new_single_node();
	retain((Object*)key);
	retain((Object*)value);
	n->key = key;
	n->value = value;
	return (Node*)n;
}

Node *single_insert(Node *self, int level, Object *key, Object *value) {
	SingleNode *node = (SingleNode*)self;

	if(node->key->hash != key->hash) {
		BitmapNode *parent = new_bitmap_node();

		retain((Object*)node);
		parent->children[(node->key->hash >> (5 * level)) & 31] = (Node*)node;

		Node *second_child = parent->children[(key->hash >> (5 * level)) & 31];
		parent->children[(key->hash >> (5 * level)) & 31] = second_child->insert(second_child, (level+1), key, value);
		return (Node*)parent;
	} else if (node->key->equal(node->key, key)) {
		SingleNode *n = new_single_node();
		n->key = key;
		n->value = value;
		return (Node*)n;
	} else {
		CollisionNode *col_node = new_collision_node();
		CollisionNode *next_col_node = new_collision_node();

		((SingleNode*)col_node)->key = node->key;
		((SingleNode*)col_node)->value = node->value;
		col_node->next = next_col_node;

		((SingleNode*)next_col_node)->key = key;
		((SingleNode*)next_col_node)->value = value;
		
		return (Node*)col_node;
	}
}

Node *bitmap_insert(Node *self, int level, Object *key, Object *value) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));
	
	int i;
	for(i=0;i<32;i++) {
		retain((Object*)node->children[i]);
	}

	Node *child = node->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->insert(child, (level+1), key, value);
	
	// we overwrote the incremented one with a new one.
	// reset the old one, we don't ref it.
	release((Object*)child);

	return (Node*)new;
}

Node *collision_insert(Node *self, int level, Object *key, Object *value) {
	CollisionNode *n = new_collision_node();
	retain((Object*)key);
	retain((Object*)value);
	((SingleNode*)n)->key = key;
	((SingleNode*)n)->value = value;
	n->next = (CollisionNode*)self;
	return (Node*)n;
}

Node *empty_remove(Node *self, int level, Object *key) {
	//retain((Object*)self);
	return self;
}

Node *single_remove(Node *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return new_empty_node();
	} else {
		retain((Object*)self);
		return self;
	}
}

Node *bitmap_remove(Node *self, int level, Object *key) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));

	int i;
	for(i=0;i<32;i++) {
		retain((Object*)node->children[i]);
	}
	
	Node *child = new->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->remove(child, (level+1), key);

	release((Object*)child);

	return (Node*)new;
}

Node *collision_remove(Node *self, int level, Object *key) {
	SingleNode *n = (SingleNode*)self;
	CollisionNode *m = (CollisionNode*)self;
	if(n->key->equal(n->key, key)) {
		if (m->next != NULL) {
			retain((Object*)m->next);
			return (Node*)m->next;
		} else {
			return NULL;
		}
	} else {
		CollisionNode *new = malloc(sizeof(CollisionNode));
		memcpy(new, m, sizeof(CollisionNode));
		new->next = (CollisionNode*)collision_remove((Node*)m->next, level, key);
		return (Node*)new;
	}
}

void print_tree(Node *n, int space) {
	int s = space;
	CollisionNode *m = (CollisionNode*)n;
	
	switch(((Object*)n)->class) {
		case SINGLENODE:
			while(s--) {
				printf(" ");
			}
			if(((SingleNode*)n)->value->class == OSTRING)
				printf("s: %u, %s\n", ((Object*)n)->refcount, ((OString*)((SingleNode*)n)->value)->str);
			else if(((SingleNode*)n)->value->class == OINT)
				printf("s: %u, %d\n", ((Object*)n)->refcount, ((OInt*)((SingleNode*)n)->value)->n);
			break;
		case COLLISIONNODE:
			for(; m != NULL; m = m->next) {
				s = space;
				while(s--) {
					printf(" ");
				}
				if(((SingleNode*)m)->value->class == OSTRING)
					printf("c: %u, %s\n", ((Object*)n)->refcount, ((OString*)((SingleNode*)m)->value)->str);
				else if(((SingleNode*)m)->value->class == OINT)
					printf("c: %u, %d\n", ((Object*)n)->refcount, ((OInt*)((SingleNode*)m)->value)->n);
			}
			break;

		case BITMAPNODE:
			printf("b:");
			for(s=0;s<32;s++) {
				print_tree(((BitmapNode*)n)->children[s], (space + 4));
			}
			break;
		default:
			while(s--) {
				printf(" ");
			}
			printf(".\n");
	}	
}

int main(int argc, char **argv) {
	Node *original = new_empty_node();

	OInt *key1 = new_oint(5);
	OString *value1 = new_ostring("hello world");
	Node *new1 = INSERT(original, key1, value1);
	OInt *key2 = new_oint(8);
	OString *value2 = new_ostring("blah");
	Node *new2 = INSERT(new1, key2, value2);

	print_tree(new1, 0);
	print_tree(new2, 0);
	release((Object*)new1);
	release((Object*)key1);
	release((Object*)value1);
	print_tree(new2, 0);
	release((Object*)new2);
	release((Object*)key2);
	release((Object*)value2);
	return 0;
}
