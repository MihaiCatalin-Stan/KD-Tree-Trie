#include "trie.h"

/*-------------------------------INITIALIZE----------------------------*/

trie_node_t *trie_create_node(trie_t *trie)
{
	trie_node_t *node = calloc(1, sizeof(trie_node_t));
	node->value = NULL;

	node->frequency = 0;
	node->end_of_word = 0;
	node->children = calloc(trie->alphabet_size, sizeof(trie_node_t *));
	node->n_children = 0;

	return node;
}

trie_t *trie_create(int data_size, int alphabet_size, char *alphabet)
{
	trie_t *trie = calloc(1, sizeof(trie_t));
	trie->root = calloc(1, sizeof(trie_node_t));

	trie->size = 0;
	trie->data_size = data_size;

	trie->alphabet_size = alphabet_size;
	trie->alphabet = calloc(alphabet_size + 1, sizeof(char));
	strcpy(trie->alphabet, alphabet);

	trie->n_nodes = 1;
	trie->root->frequency = 0;
	trie->root->value = calloc(1, trie->data_size);
	int val = -1;
	memcpy(trie->root->value, &val, trie->data_size);
	trie->root->end_of_word = 1;
	trie->root->children = calloc(trie->alphabet_size, sizeof(trie_node_t *));
	trie->root->n_children = 0;

	return trie;
}

/*--------------------------------INSERT------------------------------*/

void trie_insert(trie_t *trie, char *key, void *value)
{
	trie_node_t *cursor = trie->root;
	while (key[0] != '\0') {
		int pos = key[0] - 'a';
		if (!cursor->children[pos]) {
			cursor->children[pos] = trie_create_node(trie);
			cursor->n_children++;
			trie->n_nodes++;
		}
		cursor = cursor->children[pos];
		key++;
	}
	if (!cursor->value) {
		cursor->value = calloc(1, trie->data_size + 1);
		cursor->frequency = 1;
	} else {
		cursor->frequency++;
	}
	memcpy(cursor->value, value, trie->data_size);
	cursor->end_of_word = 1;
	trie->size++;
}

void load_words(trie_t *trie, FILE *fin)
{
	char word[MAX_LEN];
	while (fscanf(fin, "%s", word) == 1)
		trie_insert(trie, word, word);
}

/*-----------------------------REMOVE----------------------------*/

int trie_remove(trie_t *trie, trie_node_t *cursor, char *key)
{
	if (key[0] == '\0') {
		if (cursor->end_of_word) {
			cursor->frequency = 0;
			free(cursor->value);
			cursor->value = NULL;
			cursor->end_of_word = 0;
			return (cursor->n_children == 0);
		}
		return 0;
	}
	int pos = key[0] - 'a';
	key++;
	if (cursor->children[pos] &&
		trie_remove(trie, cursor->children[pos], key)) {
		free(cursor->children[pos]->children);
		free(cursor->children[pos]);
		cursor->children[pos] = NULL;
		cursor->n_children--;
		trie->n_nodes--;
		if (!cursor->n_children && !cursor->end_of_word)
			return 1;
	}
	return 0;
}

/*--------------------------AUTOCORRECT------------------------*/

void autocorrect(trie_node_t *cursor, char *key, int k, int len, int nr_difs,
				 int *ok)
{
	if (nr_difs > k)
		return;
	if (key[0] == '\0') {
		if (cursor->end_of_word) {
			// trebuie sa verificam daca exista minim o diferenta ca sa
			// fie autocorrect deci nr_difs != 0
			if (strlen((char *)cursor->value) == (size_t)len) {
				printf("%s\n", (char *)cursor->value);
				(*ok) = 1;
			}
		}
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (cursor->children[i]) {
			if (i + 'a' == key[0]) {
				autocorrect(cursor->children[i], (key + 1), k, len,
							nr_difs, ok);
			} else {
				autocorrect(cursor->children[i], (key + 1), k, len,
							(nr_difs + 1), ok);
			}
		}
	}
}

/*-------------------------AUTOCOMPLETE------------------------*/

void DFS_complete(trie_node_t *cursor, int *ok)
{
	if (cursor->end_of_word) {
		printf("%s\n", (char *)cursor->value);
		(*ok) = 1;
		return;
	}
	for (int i = 0; i < ALPHABET_SIZE && !(*ok); i++)
		if (cursor->children[i])
			DFS_complete(cursor->children[i], ok);
}

void DFS_array_complete(trie_node_t *cursor, char word[MAX_WORD][MAX_LEN],
						int *pos)
{
	if (cursor->end_of_word) {
		strcpy(word[*pos], (char *)cursor->value);
		(*pos)++;
		return;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (cursor->children[i])
			DFS_array_complete(cursor->children[i], word, pos);
}

void cmp_array_len(char word[MAX_WORD][MAX_LEN], int len)
{
	if (!len) {
		printf("No words found\n");
		return;
	}
	int min_len = strlen(word[0]), min_pos = 0;
	for (int i = 1; i < len; i++) {
		int curr_len = strlen(word[i]);
		if (curr_len < min_len) {
			min_len = curr_len;
			min_pos = i;
		}
	}
	printf("%s\n", word[min_pos]);
}

void array_complete(trie_node_t *cursor, char word[MAX_WORD][MAX_LEN],
					int frequency[MAX_WORD], int *pos)
{
	if (cursor->end_of_word) {
		strcpy(word[*pos], (char *)cursor->value);
		frequency[*pos] = cursor->frequency;
		(*pos)++;
	}
	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (cursor->children[i])
			array_complete(cursor->children[i], word, frequency, pos);
}

void cmp_array_frequency(char word[MAX_WORD][MAX_LEN], int frequency[MAX_WORD],
						 int len)
{
	if (!len) {
		printf("No words found\n");
		return;
	}
	int max_freq = frequency[0], max_pos = 0;
	for (int i = 1; i < len; i++) {
		if (frequency[i] > max_freq) {
			max_freq = frequency[i];
			max_pos = i;
		}
	}
	printf("%s\n", word[max_pos]);
}

void autocomplete(trie_node_t *cursor, char *prefix, int crit)
{
	while (prefix[0] != '\0') {
		int pos = prefix[0] - 'a';
		if (!cursor->children[pos]) {
			if (crit == 0)
				for (int i = 0; i < 3; i++)
					printf("No words found\n");
			else
				printf("No words found\n");
			return;
		}
		cursor = cursor->children[pos];
		prefix++;
	}
	int ok = 0, frequency[MAX_WORD];
	char word[MAX_WORD][MAX_LEN];
	if (crit == 1) {
		DFS_complete(cursor, &ok);
		if (!ok)
			printf("No words found\n");
	} else if (crit == 2) {
		DFS_array_complete(cursor, word, &ok);
		cmp_array_len(word, ok);
	} else if (crit == 3) {
		array_complete(cursor, word, frequency, &ok);
		cmp_array_frequency(word, frequency, ok);
	} else {
		DFS_complete(cursor, &ok);
		if (!ok)
			printf("No words found\n");
		ok = 0;
		DFS_array_complete(cursor, word, &ok);
		cmp_array_len(word, ok);
		ok = 0;
		array_complete(cursor, word, frequency, &ok);
		cmp_array_frequency(word, frequency, ok);
	}
}

/*----------------------EXIT-----------------------*/

void DFS_free(trie_t *trie, trie_node_t *cursor)
{
	for (int i = 0; i < trie->alphabet_size; i++)
		if (cursor->children[i])
			DFS_free(trie, cursor->children[i]);
	if (cursor->value) {
		free(cursor->value);
		cursor->value = NULL;
	}
	free(cursor->children);
	free(cursor);
}

void trie_free(trie_t **trie_ptr)
{
	DFS_free(*trie_ptr, (*trie_ptr)->root);
	free((*trie_ptr)->alphabet);
	free(*trie_ptr);
}
