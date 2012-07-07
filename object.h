typedef struct Object Object;
typedef unsigned int hash_t;

// hash method on Objects that returns a hash of itself. See also the hash helper function.
typedef hash_t (*hasher)(Object *obj);
typedef hash_t (*equalifier)(Object *obj, Object *other);

// everything that goes into a map should implement this
struct Object {
	hasher hash;
	equalifier equal;
};

hash_t hash(void *obj, int size);
