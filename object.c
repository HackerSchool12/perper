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

hash_t ostring_hash(Object *obj) {
	char *s = OSTR2CSTR(obj);
	return hash(s, strlen(s));
}

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->proto.hash = ostring_hash;
	//TODO add equality
	ostr->str = str;
	return ostr;
}
