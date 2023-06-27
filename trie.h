#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
#define delims " .,;:!?/'[](){}"
#define MAX_LEN 256
#define MAX_WORD 6144

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
	void *value;
	int frequency, end_of_word, n_children;
	trie_node_t **children;
};

typedef struct trie_t trie_t;
struct trie_t {
	trie_node_t *root;
	int size, data_size, alphabet_size, n_nodes;
	char *alphabet;
};

/* Functie care creeaza un nod pentru un trie, in care copiii sunt asociati
 * literelor din alfabet. Nodul si campurile sale sunt setate pe 0 si apoi este
 * returnat
 */
trie_node_t *trie_create_node(trie_t *trie);
/* Functie care creaza un trie si ii initializeaza toate campurile sale fie
 * cu 0 fie cu valorile date ca parametrii (alfabetul si dimensiunea sa).
 * Functia intoarce trie-ul creat
 */
trie_t *trie_create(int data_size, int alphabet_size, char *alphabet);
/* Procedura care insereaza un cuvant intr-un trie pe baza cheii date. Se vor
 * parcurge literele din cheie si se va adauga un copil pentru fiecare litera,
 * formand cheia pe baza acestor legaturi simbolice. In ultimul nod se va pune
 * si valoare corespunzatoare cheii
 */
void trie_insert(trie_t *trie, char *key, void *value);
/* Functie care parcurge trie-ul conform pseudocodului din laborator si care
 * elimina nodurile asociate unei chei. Daca acest nod mai are copii atunci
 * doar se seteaza end_of_word si frecventa sa pe 0
 */
int trie_remove(trie_t *trie, trie_node_t *cursor, char *key);
/* Procedura care incarca toate cuvintele dintr-un fisier trimis ca parametru
 * si le incarca pe rand in trie
 */
void load_words(trie_t *trie, FILE *fin);
// Procedura de free a unui tree care are la baza o parcurgere DFS recursiva
void DFS_free(trie_t *trie, trie_node_t *cursor);
/* Procedura care apeleaza DFS_free, si care la intoarcere elibereaza si ce a
 * a mai ramas din trie (adica alfabetul asociat si pointer-ul catre structura)
 */
void trie_free(trie_t **trie_ptr);
/* Procedura de autocorrect in care parcurgem trie-ul pana cand numarul de
 * diferente din cuvantul format in parcurgerea actuala este mai mare decat
 * cel acceptat. In cazul in care se ajunge la un end_of_word care se
 * incadreaza in acest numar, se va afisa cuvantul
 */
void autocorrect(trie_node_t *cursor, char *key, int k, int len,
				 int nr_difs, int *ok);
/* Procedura de autocomplete in care parcurgem trie-ul pana la prefixul dat
 * si pentru care apelam o alta procedura in functie de criteriu.
 * - crit = 1 -> parcurgem trie-ul cu un DFS pana la primul cuvant gasit
 * intrucat acesta va fi primul in ordine lexicografica
 *
 * - crit = 2 -> parcurgem trie-ul cu un DFS si salvam cuvintele gasite
 * intr-un vector de cuvinte, pe care il parcurgem la final
 * pentru a ne decide la cea mai mica valoare
 *
 * - crit = 3 -> parcurgem vectorul si salvam toate cuvintele, apoi apelam
 * o functie auxiliara pentru a le sorta dupa frecventa si
 * a-l afisa doar pe cel cu frecventa cea mai mare
 */
void autocomplete(trie_node_t *cursor, char *prefix, int crit);
