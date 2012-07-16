#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hashmap.h"

Node *new_empty_node() {
	static Node *n = NULL;
	if(n == NULL) {
		n = malloc(sizeof(Node));
		((Object*)n)->class = EMPTYNODE;
		n->find = empty_find;
		n->insert = empty_insert;
		n->remove = empty_remove;
	}
	return n;
}

SingleNode *new_single_node() {
	SingleNode *n = malloc(sizeof(SingleNode));
	((Object*)n)->class = SINGLENODE;
	((Node*)n)->find = single_find;
	((Node*)n)->insert = single_insert;
	((Node*)n)->remove = single_remove;
	return n;
}

BitmapNode * new_bitmap_node() {
	BitmapNode *n = malloc(sizeof(BitmapNode));
	((Object*)n)->class = BITMAPNODE;
	((Node*)n)->find = bitmap_find;
	((Node*)n)->insert = bitmap_insert;
	((Node*)n)->remove = bitmap_remove;
	int i;
	for(i=0;i<32;i++) {
		n->children[i] = new_empty_node();
	}
	return n;
}

CollisionNode * new_collision_node() {
	CollisionNode *n = malloc(sizeof(CollisionNode));
	((Object*)n)->class = COLLISIONNODE;
	n->next = NULL;
	return n;
}

Object *empty_find(Node *self, int level, Object *key) {
	return NULL;
}

Object *single_find(Node *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return ((SingleNode*)self)->value;
	} else {
		return NULL;
	}
}

Object *bitmap_find(Node *self, int level, Object *key) {
	BitmapNode *n = (BitmapNode*)self;
	Node *child = n->children[(key->hash >> (5 * level)) & 31];
	return child->find(child, (level+1), key);
}

Object *collision_find(Node *self, int level, Object *key);

Node *empty_insert(Node *self, int level, Object *key, Object *value) {
	SingleNode *n = new_single_node();
	n->key = key;
	n->value = value;
	return (Node*)n;
}

Node *single_insert(Node *self, int level, Object *key, Object *value) {
	SingleNode *node = (SingleNode*)self;

	if(node->key->hash != key->hash) {
		BitmapNode *parent = new_bitmap_node();

		parent->children[(node->key->hash >> (5 * level)) & 31] = (Node*)node;

		Node *second_child = parent->children[(key->hash >> (5 * level)) & 31];
		parent->children[(key->hash >> (5 * level)) & 31] = second_child->insert(second_child, (level+1), key, value);
		return (Node*)parent;
	} else if (node->key->equal(node->key, key)) {
		SingleNode *n = new_single_node();
		n->key = key;
		n->value = value;
		return (Node*)n;
	} else {
		CollisionNode *col_node = new_collision_node();
		CollisionNode *next_col_node = new_collision_node();

		col_node->proto = *node;
		col_node->next = next_col_node;

		((SingleNode*)next_col_node)->key = key;
		((SingleNode*)next_col_node)->value = value;
		
		return (Node*)col_node;
	}
}

Node *bitmap_insert(Node *self, int level, Object *key, Object *value) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));

	Node *child = node->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->insert(child, (level+1), key, value);

	return (Node*)new;
}

Node *collision_insert(Node *self, int level, Object *key, Object *value);

Node *empty_remove(Node *self, int level, Object *key) {
	return self;
}

Node *single_remove(Node *self, int level, Object *key) {
	if(((SingleNode*)self)->key->hash == key->hash) {
		return new_empty_node();
	} else {
		return self;
	}
}

Node *bitmap_remove(Node *self, int level, Object *key) {
	BitmapNode *node = (BitmapNode*)self;
	BitmapNode *new = malloc(sizeof(BitmapNode));
	memcpy(new, node, sizeof(BitmapNode));
	
	Node *child = new->children[(key->hash >> (5 * level)) & 31];
	new->children[(key->hash >> (5 * level)) & 31] = child->remove(child, (level+1), key);

	return (Node*)new;
}

Node *collision_remove(Node *self, int level, Object *key);

void print_tree(Node *n, int space) {
	int s = space;
	
	switch(((Object*)n)->class) {
		case SINGLENODE:
			while(s--) {
				printf(" ");
			}
			printf("s: %u, %s\n", ((SingleNode*)n)->key->hash, ((OString*)((SingleNode*)n)->value)->str);
			break;
		case BITMAPNODE:
			printf("b:");
			for(s=0;s<32;s++) {
				print_tree(((BitmapNode*)n)->children[s], (space + 4));
			}
			break;
		default:
			while(s--) {
				printf(" ");
			}
			printf(".\n");
	}	
}

int main(int argc, char **argv) {
	printf("hello world\n");
	Node *myhash = new_empty_node();
	OString *key = new_ostring("fooo baar");
	OString *value = new_ostring("baz boo");

	Node *newhash = INSERT(myhash, key, value);

	printf("the value is %s\n", OSTR2CSTR(FIND(newhash, key)));

	key = new_ostring("blah");
	value = new_ostring("jhshjda");

	Node *newnewhash = INSERT(newhash, key, value);

	Object *t = FIND(newnewhash, key);
	if(t != NULL) {
		printf("the value is %s\n", OSTR2CSTR(t));

	} else {
		printf("not found\n");
	}
	
	key = new_ostring("blIrp4iu34iurjbk");
	value = new_ostring("fooo");
	OString* value2 = new_ostring("barbazbatquux");

	Node *newnewnewhash = INSERT(newnewhash, key, value);

	printf("the value is %s here\n", OSTR2CSTR(FIND(newnewnewhash, key)));
	printf("the value is  still %s\n", OSTR2CSTR(FIND(newnewhash, new_ostring("blah"))));

	/*print_tree(newnewnewhash,0);

	Node *newnewnewnewhash = REMOVE(newnewnewhash, key);
	t = FIND(newnewhash, key);

	if(t != NULL) {
		printf("the value is %s\n", OSTR2CSTR(t));

	} else {
		printf("removed\n");
	}


	print_tree(newnewnewnewhash,0); */

	Node *gnuhash = INSERT(newnewnewhash, key, value2);

	print_tree(gnuhash, 0);

	return 0;

}
