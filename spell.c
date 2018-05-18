/* Qianqian Zheng 813288
*  17/05/2018
*/

#include <stdio.h>
#include <string.h>
#include "spell.h"
#include "hashtable.h"
#include <stdlib.h>

#define LOAD_AVERAGE 37 // average number of items per slot for hash table
#define MAXDIST 3 //maximum levenshtein distance corrected
#define NUM_ALPHABET 26
#define AVERAGE_WORD_LEN 5.1 //according to wolfram alpha
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

//helper function, compares worst case cost of searching by dictionary/ word
bool if_search_dictionary(int len, int dictionary_size, int list_size){
	// number of results deletion + substitution + addition
	float n_results = len + len * NUM_ALPHABET + (len+1)* NUM_ALPHABET;
	//cost of generating edits from a list of words
	float edit_cost = n_results * len * list_size;
	float hash_lookup_cost = edit_cost* len;
	float search_dic_cost = dictionary_size * AVERAGE_WORD_LEN * len;
	if (search_dic_cost < hash_lookup_cost) {
		return true;
	}else{
		return false;
	}
}

//scan through dictionary to find the match/close edit of a word
char* search_dictionary(List* dictionary, char* word, int dist, int max_dist){
	int size = dictionary -> size;
	int i;
	char** corrections = calloc(max_dist - dist, sizeof(char*));
	memset(corrections,0, (max_dist - dist)*sizeof(char*));

	Node *current = dictionary -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {
		int lev = levenshtein(word, current->data);
		//if distance is less than maximum distance searched for
		if (lev <= max_dist) {
			if (lev == dist) {
				free(corrections);
				return current->data;
			}
			else{
				//store possible words if none has been encountered
				if ((!corrections[lev - dist -1])&&(max_dist != dist)) {
					corrections[lev - dist -1] = current->data;
				}
			}
		}
		current = current ->next;
	}
	char* correction = calloc(1, sizeof word);
	//if there is not a word with matching distance,
	//return the word found with next smallest distance
	bool found = false;
	for ( i = 0; i < (max_dist - dist); i++) {
		if (corrections[i]) {
			correction = corrections[i];
			found = true;
			break;
		}

	}
	free(corrections);
	if (found) {
		return correction;
	}else{
		return NULL;
	}

}

//generate a list of all lower case alphabetic strings within a
//Levenshtein edit distance of 1 from the input word
List* generate_edits(char* word){
	List *edits = new_list();
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
		// printf("%s\n", edited);
		char *result = calloc(len, sizeof(char));
		strcpy(result, edited);
		// put result into list
		list_add_end(edits, result);
		free(edited);

	}

	char *copy = calloc(len+1, sizeof(char));
	//substitute a letter in the word
	for (i = 0; i < len; i++) {
		for ( j = 0; j < nAlphabet; j++) {
			strcpy(copy,word);
			copy[i] = alphabet[j];
			// printf("%s\n", copy);
			char *result = calloc(len+1, sizeof(char));
			strcpy(result, copy);
			// put result into list
			list_add_end(edits, result);
		}

	}
	free(copy);

	//add a letter in gaps
	for (i = 0; i < len+1; i++) {
		for ( j = 0; j < nAlphabet; j++) {
			char* edited = calloc(len+2, sizeof(char));
			// everything before word[i]
			if (i >= 0){
				memcpy(edited, word, i);
			}
			//add a letter
			edited[i] = alphabet[j];
			// everything after word[i]
			strcat(edited, word+i);
			// printf("%s\n", edited);
			char *result = calloc(len+2, sizeof(char));
			strcpy(result, edited);
			// put result into list
			list_add_end(edits, result);
			free(edited);
		}
	}

	return edits;
}


// see Assignment Task 2: Enumerating all possible edits
void print_all_edits(char *word) {
	int i;
	List* edits = generate_edits(word);//printing levenshtein dist of 1 here
	int size = edits -> size;
	Node *current = edits -> head;
	// iterate through edits list
	for ( i = 0; i < size; i++) {
		printf("%s\n", (char*)current->data);
		current = current ->next;
	}
	free_list( edits);

}

//for each word in the input list, generate all edits of lev dist 1 and
//return all as a list
List* edits_from_list(List* list){
	List* edits = new_list();
	int size = list -> size;
	int i;
	Node *current = list -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {
		List* temp = generate_edits(current->data);

		//add list to the end of the result list
		append_list(edits, temp);
		current = current ->next;

	}
	return edits;
}

//given a list of edits, check each against the dictionary
//and return the one occured in dictionary first
char* check_variations(List* edits, HashTable* dictionary){
	int n_variations = edits-> size;
	int closest = -1;
	char* closest_word = NULL;
	Node *current = edits -> head;
	//iterate through list of words with lev_dist+1
	int k;
	for ( k = 0; k < n_variations; k++) {
		char* correction = current -> data;
		int match= hash_table_get(dictionary, correction);

		if (match){
			if ((closest == -1)||(match<closest) ){
				closest_word = correction;
				closest = match;
			}

		}
		current= current ->next;
	}
	return closest_word;
}

//store a list linked list into hashtable, assuming there arent any
//repeating words in the dictionary
void make_hashtable(List *list, HashTable *table){

	int size = list -> size;
	int i;
	Node *current = list -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {

		hash_table_put(table, current->data, i);


		current = current ->next;
	}
}
//store a list linked list into hashtable, assuming there arent any
//repeating words in the dictionary
void make_dochashtable(List *list, DocHashTable *table){

	int size = list -> size;
	int i;
	Node *current = list -> head;
	// iterate through dictionary list
	for ( i = 0; i < size; i++) {

		hash_doctable_put(table, current->data, 0);


		current = current ->next;
	}
}

//check words in document against a distionary and
//correct them based on lev distance
void check_doc(List *dictionary, List *document, int max_lev){
	int dic_size = dictionary -> size;

	//initialize hash tables for dictionary and document
	int table_size;
	if (dic_size < LOAD_AVERAGE) {
		table_size = dic_size;
	}else{
		table_size = dic_size/LOAD_AVERAGE;
	}
	HashTable *table = new_hash_table(table_size );
	make_hashtable(dictionary, table);
	int doc_size = document -> size;
	if (doc_size < LOAD_AVERAGE) {
		table_size = doc_size;
	}else{
		table_size = doc_size/LOAD_AVERAGE;
	}
	DocHashTable *doc_table = new_hash_doctable(table_size);
	make_dochashtable(document, doc_table);

	int i;
	Node *current = document -> head;
	// iterate through document of words that needs correction
	for ( i = 0; i < doc_size; i++) {
		char* examined =  current->data;
		char* stored = doc_table_get(doc_table, examined);
		//check if we've processed the same word
		if (stored) {
			printf("%s\n", stored);
		}else{
			if (hash_table_get(table, examined)) {
				printf("%s\n", examined);

			}
			//try words with small distance in dictionary
			else{
				int j;
				bool matched = false;
				int len = strlen(examined);
				//initialize list of variations
				List* edits = new_list();
				list_add_end(edits, examined);


				for ( j = 1; (j <= max_lev)&& !matched; j++) {


					//determine if scan dictionary is faster or try variations of the word
					if (if_search_dictionary(len, dic_size, edits->size)) {

						char* match = search_dictionary(dictionary, examined, j, max_lev);
						if (match) {
							printf("%s from dictionary  levdist%d\n", match, j);
							//store in hashtable for future loopup of this word
							hash_doctable_put(doc_table, examined, match);
							matched = true;
						}
						break;
					}else{

						edits = edits_from_list(edits);

						char* match = check_variations(edits, table);
						if (match) {
							printf("%s from edits levdist%d\n", match, j);
							//store in hashtable for future loopup of this word
							hash_doctable_put(doc_table, examined, match);
							matched = true;
						}
					}


				}
				free_list(edits);
				//if no matches after the max levenshtein distance is tried
				if (!matched) {
			        char *wrong = malloc(strlen(examined)+2);
			        strcpy(wrong, examined);
			        strcat(wrong, "?");
					hash_doctable_put(doc_table, examined, wrong);
					printf("%s?\n", examined);
				}
			}
		}
		current = current ->next;
	}

	free_hash_table(table);
	free_hash_doctable(doc_table);
}

// see Assignment Task 3: Spell checking
void print_checked(List *dictionary, List *document) {
	check_doc(dictionary, document, 0); //0 is the value of lev distance
}

// see Assignment Task 4: Spelling correction
void print_corrected(List *dictionary, List *document) {
	check_doc(dictionary, document, MAXDIST); //3 is the value of lev distance
}
