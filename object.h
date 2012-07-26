#include <stdbool.h>

#pragma once

typedef struct Object Object;
typedef struct ObjectType ObjectType;
typedef struct OString OString;
typedef struct OInt OInt;
typedef unsigned int hash_t;

typedef bool (*equalifier)(Object *obj, Object *other);
typedef void (*freeer)(Object *obj);

#define OBJECTTYPEHEADER \
	equalifier equal; \
	freeer free;

#define OBJECTHEADER(type) \
	type *class; \
	hash_t hash; \
	unsigned int refcount;

struct ObjectType {
	OBJECTTYPEHEADER
};

struct Object {
	OBJECTHEADER(ObjectType)
};

struct OString {
	OBJECTHEADER(ObjectType)
	char * str;
};

struct OInt {
	OBJECTHEADER(ObjectType)
	int n;
};

#define OSTRLEN(o) strlen(OSTR2CSTR(o))
#define OSTR2CSTR(o) ((OString*)(o))->str
#define OINT2INT(i) ((OInt*)(i))->n

OString *new_ostring(char *str);
OInt *new_oint(int n);

hash_t hash(void *obj, int size);

void retain(Object *obj);

void release(Object *obj);
