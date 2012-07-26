#include "hashmap.h"

int main(int argc, char *argv[]) {
	int x;
	int y;
	OInt *key;
	OInt *val;
	BasicNode *p = new_empty_node();
	BasicNode *q;
	for(x=0;x<10000;x++) {
		for(y=0;y<1000;y++) {
			key = new_oint(y);
			val = new_oint(x);
			q = INSERT(p, key, val);
			release((Object*)key);
			release((Object*)val);
			release((Object*)p);
			p = q;
		}
	}
	release((Object*)p);
	return 0;
}
