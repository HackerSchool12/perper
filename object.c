#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "object.h"

hash_t hash(void *obj, int size) {
	hash_t hash = 0;
	int c;

	for(c = 0; c < size; c++)
		hash = *((char *) obj + c) + (hash << 6) + (hash << 16) - hash;

	return hash;
}

bool ostring_equal(Object *self, Object *other) {
	if ((self->class == other->class) && (strcmp(((OString*)self)->str, ((OString*)other)->str) == 0))
		return true;
	else
		return false;
}

char *ostring_to_string(Object *obj) {
	char *str = malloc(OSTRLEN(obj) + 2);
	sprintf(str, "\"%s\"", OSTR2CSTR(obj));
	return str;
}

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->proto.hash = hash(str, strlen(str));
	ostr->proto.class = OSTRING;
	ostr->proto.equal = ostring_equal;
	ostr->proto.to_string = ostring_to_string;
	ostr->str = str;
	return ostr;
}

bool oint_equal(Object *self, Object *other) {
	if(self->class == other->class && ((OInt*)self)->n == ((OInt*)other)->n)
		return true;
	else
		return false;
}

char *oint_to_string(Object *obj) {
	char *str = malloc(32); // chosen by fair dice roll
	sprintf(str, "%d", ((OInt*)obj)->n);
	return str;
}

OInt *new_oint(int n) {
	OInt *on = malloc(sizeof(OInt));
	((Object*)on)->hash = (hash_t)n;
	((Object*)on)->class = OINT;
	((Object*)on)->equal = oint_equal;
	((Object*)on)->to_string = oint_to_string;
	on->n = n;
	return on;
}
