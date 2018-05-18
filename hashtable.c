
/* * * * * * *
* Hash table with separate chaining in a linked list
* modified based on COMP20007 Design of Algorithms workshop 9 solutions
* by Matt Farrugia <matt.farrugia@unimelb.edu.au>
*
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

/* * * * * * *
 * Hash table with separate chaining in a linked list
 * 2 parallel tables, one with int as value and one with char* (doc table)
 * created for COMP20007 Design of Algorithms
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 *
 * move-to-front added by ...
 */



typedef struct bucket Bucket;
struct bucket {
	char *key;
	int value;
	Bucket *next;
};

Bucket *new_bucket(char *key, int value) {
	Bucket *bucket = malloc(sizeof *bucket);
	assert(bucket);

	// create own copy of key for storage in table
	bucket->key = malloc((sizeof *bucket->key) * (strlen(key) + 1));
	assert(bucket->key);
	strcpy(bucket->key, key);

	bucket->value = value;
	bucket->next = NULL;

	return bucket;
}

// Warning: does not free bucket->next
void free_bucket(Bucket *bucket) {
	assert(bucket != NULL);
	free(bucket->key);
	free(bucket);
}

/* * *
* HELPER DATA STRUCTURE: LINKED LIST OF BUCKETS
*/

typedef struct docbucket Docbucket;
struct docbucket {
	char *key;
	char *value;
	Docbucket *next;
};

Docbucket *new_docbucket(char *key, char *value) {
	Docbucket *bucket = malloc(sizeof *bucket);
	assert(bucket);

	// create own copy of key for storage in table
	bucket->key = malloc((sizeof *bucket->key) * (strlen(key) + 1));
	assert(bucket->key);
	strcpy(bucket->key, key);
	bucket->value = malloc((sizeof *bucket->key) * (strlen(key) + 5));

	bucket->next = NULL;

	return bucket;
}

// Warning: does not free bucket->next
void free_docbucket(Docbucket *bucket) {
	assert(bucket != NULL);
	free(bucket->key);
	// free(bucket->value);

	free(bucket);
}

struct table {
	int size;
	Bucket **buckets;
};


/* * *
 * HASH TABLE CREATION/DELETION
 */

HashTable *new_hash_table(int size) {
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

void free_hash_table(HashTable *table) {
	assert(table != NULL);

	int i;
	for (i = 0; i < table->size; i++) {
		Bucket *this_bucket, *next_bucket;
		this_bucket = table->buckets[i];
		while (this_bucket) {
			next_bucket = this_bucket->next;
			free_bucket(this_bucket);
			this_bucket = next_bucket;
		}
	}

	free(table);
}


struct doctable {
	int size;
	Docbucket **buckets;
};


/* * *
* HASH TABLE CREATION/DELETION
*/

DocHashTable *new_hash_doctable(int size ) {
	DocHashTable *table = malloc(sizeof *table);
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


void free_hash_doctable(DocHashTable *table) {
	assert(table != NULL);
	int i;
	for (i = 0; i < table->size; i++) {
		Docbucket *this_bucket, *next_bucket;
		this_bucket = table->buckets[i];
		while (this_bucket) {
			next_bucket = this_bucket->next;
			free_docbucket(this_bucket);
			this_bucket = next_bucket;
		}

	}
	free(table->buckets);
	free(table);
}


/* * *
 * HASHING HELPER FUNCTIONS
 */

 // xor hash from lectures, with seed 73802
 unsigned int seed = 73802;
 unsigned int h(const char *key, unsigned int size) {
 	unsigned int h = seed;

 	int i;
 	for (i = 0; key[i] != '\0'; i++) {
 		h = h ^ ((h << 5) + key[i] + (h >> 2));
 	}

 	return h % size;
 }

bool equal(char *a, char *b) {
	return strcmp(a, b) == 0;
}


/* * *
 * HASH TABLE FUNCTIONS
 */


void hash_table_put(HashTable *table, char *key, int value) {

	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);

	// add it at front of list
	Bucket *new = new_bucket(key, value);
	new->next = table->buckets[hash_value];
	table->buckets[hash_value] = new;
}

int hash_table_get(HashTable *table, char *key) {
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
			return bucket->value;
		}
		previous = bucket;
		bucket = bucket->next;
	}

	// if key doesn't exist
	return 0;
}

/* * *
* HASH TABLE FUNCTIONS
*/

void hash_doctable_put(DocHashTable *table, char *key, char *value) {

	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);


		// look for existing key
		Docbucket *bucket = table->buckets[hash_value];
		Docbucket *previous = NULL;
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
		Docbucket *new = new_docbucket(key, value);
		new->next = table->buckets[hash_value];
		table->buckets[hash_value] = new;

}

char* doc_table_get(DocHashTable *table, char *key) {

	assert(table != NULL);
	assert(key != NULL);

	int hash_value = h(key, table->size);

	// look for existing key
	Docbucket *bucket = table->buckets[hash_value];
	Docbucket *previous = NULL;
	while (bucket) {
		if (equal(key, bucket->key)) {

			// move to front on access
			if (previous) {
				previous->next = bucket->next;
				bucket->next = table->buckets[hash_value];
				table->buckets[hash_value] = bucket;
			}
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

	// key doesn't exist
	return 0;
}

// /* * *
// * PRINTING FUNCTIONS
// */
//
// void print_hash_table(HashTable *table) {
// 	assert(table != NULL);
// 	fprint_hash_table(stdout, table);
// }
//
// void fprint_hash_table(FILE *file, HashTable *table) {
// 	assert(table != NULL);
//
// 	// max width of a table row label
// 	int width = snprintf(NULL, 0, "%d", table->size-1);
//
// 	int i, j;
// 	for (i = 0; i < table->size; i++) {
// 		fprintf(file, " %*d|", width, i);
//
// 		j = 0;
// 		Bucket *bucket;
// 		bucket = table->buckets[i];
// 		while (bucket && j < PRINT_LIMIT) {
// 			fprintf(file, "->(\"%s\": %s)", bucket->key, bucket->value);
// 			bucket = bucket->next;
// 			j++;
// 		}
// 		if (bucket) {
// 			fprintf(file, "...");
// 		}
// 		fprintf(file, "\n");
// 	}
// }
