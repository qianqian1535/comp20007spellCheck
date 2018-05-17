/* ... */

#include <stdio.h>
#include <string.h>
#include "spell.h"
#include "hashtable.h"
#include <stdlib.h>

#define LOAD_AVERAGE 13 // average number of items per slot for hash table
#define MAXDIST 3 //maximum levenshtein distance corrected
#define NUM_ALPHABET 26
//helper function, minimum of 2
int min(int a, int b){
	if (a < b){
		return a;
	}else{
		return b;
	}
}
//helper function, minimum of 3 ints
int min_three(int a, int b,int c){
	int temp = min(a,b);
	if (temp < c){
		return temp;
	}else{
		return c;
	}
}

void print2darray(int **grid, int row, int col){
	int i, j;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {

			printf("%2d ", grid[i][j]);
		}
		printf("\n");
	}
}
// this function is based on week 7 tutorial solution for levenshtein distance
int levenshtein(char *word1, char *word2){
	int n = strlen(word1)+1; //array row size
	int m = strlen(word2)+1; //array column size
	// allocate the 2d array
	int **E = calloc(n , sizeof(int*));
	int i, j;
	for(i = 0; i < n; i++){
		E[i] = calloc(m, sizeof(int));
	}
	// E[i, j] is the edit distance between the first i characters of word_1
	// and the first j characters of word_2

	// fill in known values
	for (i = 0; i < n; i++ ){
		E[i][0] = i;
	}
	for (j = 1; j < m; j++ ){
		E[0][j] = j;
	}
	// then fill in the rest of the table
	for (i = 1; i < n; i++) {
		for ( j = 1; j < m; j++) {
			int substitute_cost;
			//if the corresponding letters are the same
			if ( word1[i-1] == word2[j-1]){
				substitute_cost = 0;
			}else {
				substitute_cost = 1;
			}
			E[i][j] = min_three(
				E[i-1][j-1] + substitute_cost, // substitution
				E[i-1][j] + 1, // deletion
				E[i][j-1] + 1 // insertion
			);
		}
	}
	//print2darray(E, n, m);
	int dist = E[n-1][m-1];

	// free the 2d array
	for(i = 0; i < n; i++){
		free(E[i]);
	}
	free(E);

	return dist;
}

void print_edit_distance(char *word1, char *word2) {
	printf("%d\n", levenshtein(word1, word2));
}

//helper function, returns number of edits with dist 1
int num_edits(int len){
	// number of results deletion + substitution + addition
	int n_results = len + len * NUM_ALPHABET + (len+1)* NUM_ALPHABET;
	return n_results;
}

//generate all lower case alphabetic strings within a
//Levenshtein edit distance of 1 from the input word
void all_edits(char* word, char** edits){
	static const char* alphabet = "abcdefghijklmnopqrstuvwxyz";
	const int nAlphabet = 26;
	int len = strlen(word);
	int count = 0;
	int i, j;
	//delete a letter from word
	for (i = 0; i < len; i++) {
		char* edited = calloc(len, sizeof(char));
		// everything before word[i]
		if (i >= 0){
			memcpy(edited, word, i);
		}
		// everything after word[i]
		strcat(edited, word+i+1);
		// copy result into array
		strcpy(edits[count],edited);
		free(edited);
		count++;
	}

	char *copy = malloc(strlen(word)+1);
	//substitute a letter in the word
	for (i = 0; i < len; i++) {
		for ( j = 0; j < nAlphabet; j++) {
			strcpy(copy,word);
			copy[i] = alphabet[j];
			strcpy(edits[count],copy);
			(count)++;
		}

	}
	free(copy);

	//add a letter in gaps
	for (i = 0; i < len+1; i++) {
		for ( j = 0; j < nAlphabet; j++) {
			char* edited = calloc(len, sizeof(char));
			// everything before word[i]
			if (i >= 0){
				memcpy(edited, word, i);
			}
			//add a letter
			edited[i] = alphabet[j];
			// everything after word[i]
			strcat(edited, word+i);
			// copy result into array
			strcpy(edits[count],edited);
			free(edited);
			count++;
		}
	}
}

void freeStringArray(char** edits, char* word){
	int i, n_results = num_edits(strlen(word));
	for(i = 0; i < n_results; i++){
		free(edits[i]);
	}
	free(edits);
}

char** generate_edits(char *word){

	int len = strlen(word);
	int n_results = num_edits(len);
	//initialise string array to store all the edits
	char** edits = calloc( n_results+1 ,sizeof(char*));

	int i;
	for(i = 0; i < n_results; i++){
		//allocate size of word length + 2 to each element
		edits[i] = calloc(len+2, sizeof(char));
	}
	char** edits_copy = edits;
	all_edits(word, edits_copy);

	return edits_copy;
}

// see Assignment Task 2: Enumerating all possible edits
void print_all_edits(char *word) {
	int i, n_results = num_edits(strlen(word));
	char** edits = generate_edits(word);//printing levenshtein dist of 1 here
	for ( i = 0; i < n_results; i++) {
		printf("%s\n", edits[i]);
	}
	freeStringArray( edits,  word);

}

//store a list linked list into hashtable, assuming there arent any
//repeating words in the dictionary
void make_hashtable(List *list, HashTable *table, bool dictionary){

	int size = list -> size;
	int i;
	Node *current = list -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {
		hash_table_put(table, current->data, 0, dictionary);
		current = current ->next;
	}
}



void check_docs(List *dictionary, List *document, int max_lev){
	int dic_size = dictionary -> size;

	HashTable *table = new_hash_table(dic_size/LOAD_AVERAGE, true);
	make_hashtable(dictionary, table, true);
	int doc_size = document -> size;
	HashTable *doc_table = new_hash_table(doc_size/LOAD_AVERAGE, false);
	make_hashtable(document, doc_table, false);
	// print_hash_table(doc_words);

	int i;
	Node *current = document -> head;
	// iterate through dictionary list
	for ( i = 0; i < doc_size; i++) {
		char* examined =  current->data;

		char* stored = hash_table_get(doc_table, examined);
		//check if we've processed the same word
		if (stored) {
			printf("%s\n", stored);
		}else{

			int j;
			int n_variations;
			char** edits = malloc(sizeof(char*));//list of possible corrections
			edits[0] = examined;
			// printf("assigned edits %s\n", edits[0]);
			bool matched = false;
			//
			for ( j = 0; (j <= max_lev)&& !matched; j++) {
				n_variations = sizeof(edits)/sizeof(edits[0]);
				// printf("n_variations %d\n", n_variations);
				if (j == 0) {
					// if lev distance is 0, the number of words searched is 1
					n_variations = 1;
				}
				// else{
				// 	n_variations =
				// }
				int k;
				for ( k = 0; k < n_variations; k++) {
					char* correction = edits[k];
					// printf("before key is null assigned edits %s\n", edits[0]);
					bool match = hash_table_has(table, correction);
					if (match){
						printf("%s\n", correction);
						//store in hashtable ofr future loopup of this word
						hash_table_put(doc_table, examined, correction, false);
						matched = true;
						break;
					}else{
						//consider using list for edits
					}
				}

			}
			free(edits);
			//if no matches after the max levenshtein distance is tried
			if (!matched) {
				printf("%s?\n", examined);
			}
		}
		current = current ->next;
	}

	free_hash_table(table, true);
	free_hash_table(doc_table, false);
}

// see Assignment Task 3: Spell checking
void print_checked(List *dictionary, List *document) {
	check_docs(dictionary, document, 0); //0 is the value of lev distance
}

// see Assignment Task 4: Spelling correction
void print_corrected(List *dictionary, List *document) {
	check_docs(dictionary, document, 3); //0 is the value of lev distance
}
