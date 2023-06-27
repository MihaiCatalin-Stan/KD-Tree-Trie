#include "trie.h"

int main(void)
{
	int k;
	char *op = calloc(MAX_LEN, sizeof(char)),
	*word = calloc(MAX_LEN, sizeof(char));
	trie_t *trie = trie_create(MAX_LEN, ALPHABET_SIZE, ALPHABET);
	while (1) {
		scanf("%s", op);
		if (!strcmp(op, "LOAD")) {
			scanf("%s", word);
			FILE *fin = fopen(word, "r");
			load_words(trie, fin);
			fclose(fin);
			// printf("loaded");
			memset(word, 0, MAX_LEN);
		} else if (!strcmp(op, "INSERT")) {
			scanf("%s", word);
			trie_insert(trie, word, word);
			memset(word, 0, MAX_LEN);
		} else if (!strcmp(op, "REMOVE")) {
			scanf("%s", word);
			trie_remove(trie, trie->root, word);
			memset(word, 0, MAX_LEN);
		} else if (!strcmp(op, "AUTOCORRECT")) {
			int ok = 0;
			scanf("%s %d", word, &k);
			autocorrect(trie->root, word, k, strlen(word), 0, &ok);
			if (!ok)
				printf("No words found\n");
			memset(word, 0, MAX_LEN);
		} else if (!strcmp(op, "AUTOCOMPLETE")) {
			scanf("%s %d", word, &k);
			autocomplete(trie->root, word, k);
			memset(word, 0, MAX_LEN);
		} else if (!strcmp(op, "EXIT")) {
			free(op);
			free(word);
			trie_free(&trie);
			exit(0);
		}
	}
	return 0;
}
