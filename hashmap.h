#include "object.h"

#pragma once

typedef const struct NodeType NodeType;
typedef struct BasicNode BasicNode;
typedef struct SingleNode SingleNode;
typedef struct BitmapNode BitmapNode;
typedef struct CollisionNode CollisionNode;

// lookup this Object key in itself using Object->hash
typedef Object *(*finder)(BasicNode *self, int level, Object *key);
// insert value into Node under key
typedef BasicNode *(*inserter)(BasicNode *self, int level, Object *key, Object *value);
// remove key from self
typedef BasicNode *(*remover)(BasicNode *self, int level, Object *key);

struct NodeType {
	OBJECTTYPEHEADER
	finder find;
	inserter insert;
	remover remove;
};

#define SINGLENODEHEADER \
	OBJECTHEADER(NodeType) \
	Object *key; \
	Object *value;

struct BasicNode {
	OBJECTHEADER(NodeType)
};

// the single node contains just one key/value
// find retursn the value or NULL
// insert returns a BitmapNode
// remove returns itself or the empty Node.
struct SingleNode {
	SINGLENODEHEADER
};

// A linked list of SingleNodes with the same hash
struct CollisionNode {
	SINGLENODEHEADER
	CollisionNode *next;
};

// Initialized with 32 empty nodes indexed by hash
// takes the first 5 bytes of the hash of key
// calls the respective function on that key, bitshifting the hash.
struct BitmapNode {
	OBJECTHEADER(NodeType)
	BasicNode *children[32]; //TODO investigate dynamic array
};


#define INSERT(map, key, value) ((BasicNode*)map)->class->insert(((BasicNode*)map), 0, ((Object*)key), ((Object*)value))

#define FIND(map, key) ((BasicNode*)map)->class->find(((Object*)map), 0, ((BasicNode*)key))

#define REMOVE(map, key) ((BasicNode*)map)->class->remove(((BasicNode*)map), 0, ((Object*)key))

BasicNode *new_empty_node(void);
SingleNode *new_single_node(void);
BitmapNode *new_bitmap_node(void);
CollisionNode *new_collision_node(void);
