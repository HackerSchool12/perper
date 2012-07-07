typedef struct Object Object;
typedef struct Node Node;
typedef struct SingleNode SingleNode;
typedef struct BitmapNode BitmapNode;
typedef struct CollisionNode CollisionNode;

typedef unsigned int hash_t;

// hash method on Objects that returns a hash of itself. See also the hash helper function.
typedef hash_t (*hasher)(Object *obj);
typedef hash_t (*equalifier)(Object *obj, Object *other);

// lookup this Object key in itself using Object->hash
typedef Object *(*finder)(Node *self, int level, hash_t hash, Object *key);
// insert value into Node under key
typedef Node *(*inserter)(Node *self, int level, hash_t hash, Object *key, Object *value);
// remove key from self
typedef Node *(*remover)(Node *self, int level, hash_t hash, Object *key);

// everything that goes into a map should implement this
struct Object {
	hasher hash;
	equalifier equal;
};

// The abstract/empty node. All nodes implement this interface.
// Node is an Object, which allows maps to be keys and values
struct Node {
	Object proto;
	// always NULL
	finder find;
	// return a SingleNode
	inserter insert;
	// always returns the EmprtyNode
	remover remove;
};

// the single node contains just one key/value
// find retursn the value or NULL
// insert returns a BitmapNode
// remove returns itself or the empty Node.
struct SingleNode {
	Node proto;
	Object *key;
	hash_t hash;
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
	int count;
	Node *children[32];
};

hash_t hash(Object *obj, int size);

Node *new_empty_node();
SingleNode *new_single_node();
BitmapNode * new_bitmap_node();
CollisionNode * new_collision_node();

Object *empty_find(Node *self, int level, hash_t hash, Object *key);
Object *single_find(Node *self, int level, hash_t hash, Object *key);
Object *bitmap_find(Node *self, int level, hash_t hash, Object *key);
Object *collision_find(Node *self, int level, hash_t hash, Object *key);

Node *empty_insert(Node *self, int level, hash_t hash, Object *key, Object *value);
Node *single_insert(Node *self, int level, hash_t hash, Object *key, Object *value);
Node *bitmap_insert(Node *self, int level, hash_t hash, Object *key, Object *value);
Node *collision_insert(Node *self, int level, hash_t hash, Object *key, Object *value);

Node *empty_remove(Node *self, int level, hash_t hash, Object *key);
Node *single_remove(Node *self, int level, hash_t hash, Object *key);
Node *bitmap_remove(Node *self, int level, hash_t hash, Object *key);
Node *collision_remove(Node *self, int level, hash_t hash, Object *key);
