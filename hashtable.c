
/* * * * * * *
* Hash table with separate chaining in a linked list
*
* created for COMP20007 Design of Algorithms
* by Matt Farrugia <matt.farrugia@unimelb.edu.au>
*
* move-to-front added by ...
*/
//end_t = 5072786 both table, just compare


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hashtable.h"

#include <string.h>
#include "strhash.h"
#define HASH_METHOD 'x'

#define PRINT_LIMIT 10


/* * *
* HELPER DATA STRUCTURE: LINKED LIST OF BUCKETS
*/

typedef struct bucket Bucket;
struct bucket {
	char *key;
	char *value;
	Bucket *next;
};

Bucket *new_bucket(char *key, char *value,  bool dictionary) {
	Bucket *bucket = malloc(sizeof *bucket);
	assert(bucket);

	// create own copy of key for storage in table
	bucket->key = malloc((sizeof *bucket->key) * (strlen(key) + 1));
	assert(bucket->key);
	strcpy(bucket->key, key);
	if (!dictionary){
		bucket->value = malloc((sizeof *bucket->key) * (strlen(key) + 5));
	}

	bucket->next = NULL;

	return bucket;
}

// Warning: does not free bucket->next
void free_bucket(Bucket *bucket, bool dictionary) {
	assert(bucket != NULL);
	free(bucket->key);
	// if (!dictionary){
	// 	free(bucket->value);
	// }
	free(bucket);
}

struct table {
	int size;
	Bucket **buckets;
};


/* * *
* HASH TABLE CREATION/DELETION
*/

HashTable *new_hash_table(int size,  bool dictionary) {
	HashTable *table = malloc(sizeof *table);
	assert(table);

	table->size = size;
	table->buckets = malloc(size * (sizeof *table->buckets));
	assert(table->buckets);
	int i;
	for (i = 0; i < size; i++) {
		table->buckets[i] = NULL;
	}

	return table;
}

void free_hash_table(HashTable *table,  bool dictionary) {
	assert(table != NULL);

	int i;
	for (i = 0; i < table->size; i++) {
		Bucket *this_bucket, *next_bucket;
		this_bucket = table->buckets[i];
		while (this_bucket) {
			next_bucket = this_bucket->next;
			free_bucket(this_bucket, dictionary);
			this_bucket = next_bucket;
		}
	}

	free(table);
}


/* * *
* HASHING HELPER FUNCTIONS
*/

int h(char *key, int size) {
	return hash(key, size, HASH_METHOD);
}
bool equal(char *a, char *b) {
	return strcmp(a, b) == 0;
}


/* * *
* HASH TABLE FUNCTIONS
*/

void hash_table_put(HashTable *table, char *key, char *value, bool dictionary) {

	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);

	if(dictionary){
		//assuming every word will be unique in dictionary
		Bucket *new = new_bucket(key, value, dictionary);
		new->next = table->buckets[hash_value];
		table->buckets[hash_value] = new;
	}else{
		// look for existing key
		Bucket *bucket = table->buckets[hash_value];
		Bucket *previous = NULL;
		while (bucket) {
			if (equal(key, bucket->key)) {

				// move to front on modify
				if (previous) {
					previous->next = bucket->next;
					bucket->next = table->buckets[hash_value];
					table->buckets[hash_value] = bucket;
				}
				bucket->value = value;
				return;
			}
			previous = bucket;
			bucket = bucket->next;
		}

		// if key wasn't found, add it at front of list
		Bucket *new = new_bucket(key, value, dictionary);
		new->next = table->buckets[hash_value];
		table->buckets[hash_value] = new;
	}
}

char* hash_table_get(HashTable *table, char *key) {

	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);

	// look for existing key
	Bucket *bucket = table->buckets[hash_value];
	Bucket *previous = NULL;
	while (bucket) {
		if (equal(key, bucket->key)) {

			// move to front on access
			if (previous) {
				previous->next = bucket->next;
				bucket->next = table->buckets[hash_value];
				table->buckets[hash_value] = bucket;
			}
			// printf(" getting %s value %s\n",key, bucket->value);
			// return bucket->value;
			if (bucket->value) {
				if(strlen(bucket->value)> 0 ){
					return bucket->value;
				}
			}else{
				return 0;
			}
		}
		previous = bucket;
		bucket = bucket->next;
	}

	// key doesn't exist!
	// fprintf(stderr, "error: key \"%s\" not found in table\n", key);
	return 0;
}

bool hash_table_has(HashTable *table, char *key) {
	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);

	// look for existing key
	Bucket *bucket = table->buckets[hash_value];
	Bucket *previous = NULL;
	while (bucket) {
		if (equal(key, bucket->key)) {

			// move to front on lookup
			if (previous) {
				previous->next = bucket->next;
				bucket->next = table->buckets[hash_value];
				table->buckets[hash_value] = bucket;
			}

			return true;
		}
		previous = bucket;
		bucket = bucket->next;
	}

	// key doesn't exist!
	return false;
}
/* * *
* PRINTING FUNCTIONS
*/

void print_hash_table(HashTable *table) {
	assert(table != NULL);
	fprint_hash_table(stdout, table);
}

void fprint_hash_table(FILE *file, HashTable *table) {
	assert(table != NULL);

	// max width of a table row label
	int width = snprintf(NULL, 0, "%d", table->size-1);

	int i, j;
	for (i = 0; i < table->size; i++) {
		fprintf(file, " %*d|", width, i);

		j = 0;
		Bucket *bucket;
		bucket = table->buckets[i];
		while (bucket && j < PRINT_LIMIT) {
			fprintf(file, "->(\"%s\": %s)", bucket->key, bucket->value);
			bucket = bucket->next;
			j++;
		}
		if (bucket) {
			fprintf(file, "...");
		}
		fprintf(file, "\n");
	}
}
