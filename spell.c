/* ... */

#include <stdio.h>
#include <string.h>
#include "spell.h"
#include "hashtable.h"
#include <stdlib.h>

#define LOAD_AVERAGE 100 // average number of items per slot for hash table

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

//generate and print all lower case alphabetic strings within a
//Levenshtein edit distance of 1 from the input word
void all_edits(char* word, int *count, char** edits){
	static const char* alphabet = "abcdefghijklmnopqrstuvwxyz";
	const int nAlphabet = 26;
	int len = strlen(word);

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
		strcpy(edits[*count],edited);
		free(edited);
		(*count)++;
	}

	char *copy = malloc(strlen(word)+1);
	//substitute a letter in the word
	for (i = 0; i < len; i++) {
		for ( j = 0; j < nAlphabet; j++) {
			strcpy(copy,word);
			copy[i] = alphabet[j];
			strcpy(edits[*count],copy);
			(*count)++;
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
			strcpy(edits[*count],edited);
			free(edited);
			(*count)++;
		}
	}
}
// see Assignment Task 2: Enumerating all possible edits
void print_all_edits(char *word) {
	int *count = malloc(sizeof *count);
	*count = 0;
	const int nAlphabet = 26;
	int len = strlen(word);
	// number of results deletion + substitution + addition
	int n_results = len + len * nAlphabet + (len+1)* nAlphabet;
	//initialise string array to store all the edits
	char** edits = calloc( n_results+1 ,sizeof(char*));

	int i;
	for(i = 0; i < n_results; i++){
		//allocate size of word length + 2 to each element
		edits[i] = calloc(len+2, sizeof(char));
	}
	char** edits_copy = edits;
	all_edits(word, count, edits_copy);
	for ( i = 0; i < *count; i++) {
		printf("%s\n", edits[i]);
	}
	// free string array
	for(i = 0; i < n_results; i++){
		free(edits[i]);
	}
	free(edits);
	free(count);
}

//store a dictionary linked list into hashtable, assuming there arent any
//repeating words in the dictionary
void hash_dictionary(List *dictionary, HashTable *table){

	int size = dictionary -> size;
	int i;
	Node *current = dictionary -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {
		hash_table_put(table, current->data, 0);
		current = current ->next;
	}
}
void hash_doc(List *document, HashTable *table){

}
void check_docs(List *dictionary, List *document){
	int dic_size = dictionary -> size;

	HashTable *table = new_hash_table(dic_size/LOAD_AVERAGE);
	hash_dictionary(dictionary, table);
	print_hash_table(table);

	int doc_size = document -> size;
	//HashTable *doc_words = new_hash_table(doc_size/LOAD_AVERAGE);
	free_hash_table(table);
//	free_hash_table(doc_words);
}
// see Assignment Task 3: Spell checking
void print_checked(List *dictionary, List *document) {
	check_docs(dictionary, document);
}

// see Assignment Task 4: Spelling correction
void print_corrected(List *dictionary, List *document) {
	printf("not yet implemented: put code for task 4 here\n");
}
