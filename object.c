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

bool ostring_equal(Object *self, Object *other) {
	if ((self->class == other->class) && (strcmp(((OString*)self)->str, ((OString*)other)->str) == 0))
		return true;
	else
		return false;
}

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->proto.hash = hash(str, strlen(str));
	ostr->proto.class = OSTRING;
	ostr->proto.equal = ostring_equal;
	ostr->str = str;
	return ostr;
}
