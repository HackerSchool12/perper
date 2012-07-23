#include "object.h"

#pragma once

typedef struct Node Node;
typedef struct SingleNode SingleNode;
typedef struct BitmapNode BitmapNode;
typedef struct CollisionNode CollisionNode;

// lookup this Object key in itself using Object->hash
typedef Object *(*finder)(Node *self, int level, Object *key);
// insert value into Node under key
typedef Node *(*inserter)(Node *self, int level, Object *key, Object *value);
// remove key from self
typedef Node *(*remover)(Node *self, int level, Object *key);

// The abstract/empty node. All nodes implement this interface.
// Node is an Object, which allows maps to be keys and values
struct Node {
	Object proto;
	// always NULL
	finder find;
	// return a SingleNode
	inserter insert;
	// always returns the empty node
	remover remove;
};

// the single node contains just one key/value
// find retursn the value or NULL
// insert returns a BitmapNode
// remove returns itself or the empty Node.
struct SingleNode {
	Node proto;
	Object *key;
	Object *value;
};

// A linked list of SingleNodes with the same hash
struct CollisionNode {
	SingleNode proto;
	CollisionNode *next;
};

// Initialized with 32 empty nodes indexed by hash
// takes the first 5 bytes of the hash of key
// calls the respective function on that key, bitshifting the hash.
struct BitmapNode {
	Node proto;
	Node *children[32];
};


#define INSERT(map, key, value) ((Node*)map)->insert(((Node*)map), 0, ((Object*)key), ((Object*)value))

#define FIND(map, key) ((Node*)map)->find(((Node*)map), 0, ((Object*)key))

#define REMOVE(map, key) ((Node*)map)->remove(((Node*)map), 0, ((Object*)key))

Node *new_empty_node(void);
SingleNode *new_single_node(void);
BitmapNode * new_bitmap_node(void);
CollisionNode * new_collision_node(void);
