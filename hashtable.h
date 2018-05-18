
/* * * * * * *
* Hash table with separate chaining in a linked list
* modified based on COMP20007 Design of Algorithms workshop 9 solutions
* by Matt Farrugia <matt.farrugia@unimelb.edu.au>
*
*/

#include <stdbool.h>

typedef struct table HashTable;
typedef struct doctable DocHashTable;

HashTable *new_hash_table(int size);
void free_hash_table(HashTable *table);

DocHashTable *new_hash_doctable(int size );
void free_hash_doctable(DocHashTable *table);

void hash_table_put(HashTable *table, char *key, int value);
int hash_table_get(HashTable *table, char *key);

void hash_doctable_put(DocHashTable *table, char *key, char *value);
char* doc_table_get(DocHashTable *table, char *key);

// void print_hash_table(HashTable *table);
// void fprint_hash_table(FILE *file, HashTable *table);
