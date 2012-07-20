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
		obj->free(obj);
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

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->proto.hash = hash(str, strlen(str));
	ostr->proto.class = OSTRING;
	ostr->proto.equal = ostring_equal;
	ostr->str = str;
	ostr->proto.refcount = 1;
	ostr->proto.free = ostring_free;
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

OInt *new_oint(int n) {
	OInt *on = malloc(sizeof(OInt));
	((Object*)on)->hash = (hash_t)n;
	((Object*)on)->class = OINT;
	((Object*)on)->equal = oint_equal;
	on->n = n;
	((Object*)on)->refcount = 1;
	((Object*)on)->free = oint_free;
	return on;
}
