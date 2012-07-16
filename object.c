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

OString *new_ostring(char *str) {
	OString *ostr = malloc(sizeof(OString));
	ostr->proto.hash = hash(str, strlen(str));
	ostr->proto.class = OSTRING;
	//TODO add equality
	ostr->str = str;
	return ostr;
}
