#include <stdbool.h>

typedef struct Object Object;
typedef struct OString OString;
typedef unsigned int hash_t;

typedef enum class_t {
	OBJECT,
	EMPTYNODE,
	SINGLENODE,
	BITMAPNODE,
	COLLISIONNODE,
	OSTRING,
} class_t;

typedef bool (*equalifier)(Object *obj, Object *other);

// everything that goes into a map should implement this
struct Object {
	class_t class;
	hash_t hash;
	equalifier equal;
};

struct OString {
	Object proto;
	char * str;
};

#define OSTRLEN(o) strlen(OSTR2CSTR(o))
#define OSTR2CSTR(o) ((OString*)(o))->str

OString *new_ostring(char *str);

hash_t hash(void *obj, int size);

