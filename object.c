#include <string.h>
#include <stdlib.h>
#include "object.h"

hash_t hash(void *obj, int size) {
	hash_t hash = 0;
	int c;

	for(c = 0; c < size; c++)
		hash = *((char *) obj + c) + (hash << 6) + (hash << 16) - hash;

	return hash;
}

void retain(Object *obj) {
	obj->refcount++;
}

void release(Object *obj) {
	obj->refcount--;
	if(obj->refcount == 0) {
		obj->class->free(obj);
	}
}

bool ostring_equal(Object *self, Object *other) {
	if ((self->class == other->class) && (strcmp(((OString*)self)->str, ((OString*)other)->str) == 0))
		return true;
	else
		return false;
}

void ostring_free(Object *obj) {
	OString *s = (OString*)obj;
	//free(s->str);
	free(s);
}

ObjectType ostring_type = {ostring_equal, ostring_free};

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->hash = hash(str, strlen(str));
	ostr->class = &ostring_type;
	ostr->str = str;
	ostr->refcount = 1;
	return ostr;
}

bool oint_equal(Object *self, Object *other) {
	if(self->class == other->class && ((OInt*)self)->n == ((OInt*)other)->n)
		return true;
	else
		return false;
}

void oint_free(Object *obj) {
	free(obj);
}

ObjectType oint_type = {oint_equal, oint_free};

OInt *new_oint(int n) {
	OInt *on = malloc(sizeof(OInt));
	on->hash = (hash_t)n;
	on->class = &oint_type;
	on->n = n;
	on->refcount = 1;
	return on;
}
