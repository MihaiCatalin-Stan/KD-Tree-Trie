#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_LEN 256
#define INF 999999999

typedef struct kd_node_t kd_node_t;
struct kd_node_t {
	kd_node_t *right, *left;
	int *dims;
};

typedef struct kd_tree_t kd_tree_t;
struct kd_tree_t {
	kd_node_t *root;
	int no_of_dims;
};

/* Functie care aloca spatiu pentru un nod si il initializeaza.
 * Fiii nodului sunt initializati pe NULL, iar vectorul de coordonate
 * al acestuia sunt initializate cu 0
 */
kd_node_t *kd_create_node(kd_tree_t *tree);
/* Functie care creaza un kd-tree, il initializeaza cu root-ul pe NULL
 * si cu numarul de dimensiuni necesare
 */
kd_tree_t *kd_create(int no_of_dims);
/* Procedura care adauga un nod intr-un kd-tree. Parcurge tree-ul dupa regula
 * data. Nodurile care au coordonata dupa care se imparte arborele mai mare
 * sau egale decat a nodului curent vor fi plasate la dreapta sa, iar celelalte
 * la stanga. Abordam si cazul separat pt un root = NULL
 */
void kd_tree_insert(kd_tree_t *tree, kd_node_t *new_node, int curr_level);
/* Procedura care incarca datele din fisier (initial n si nr de dimensiuni)
 * si apoi coordonatele pentru toate nodurile din arbore. Dupa fiecare citire
 * se apeleaza functia de insert
 */
void load_data(kd_tree_t **tree, FILE *fin, int *n);
/* Functie care cauta punctul cel mai apropiat de un punct dat. Coordonatele
 * punctului cautat sunt stocate in v. Vom parcurge arborele pana vom ajunge
 * pe frunza corespunztoare coordonatelor lui V. De aici ne intoarcem din
 * recursivitate.
 * Vom verifica daca distanta din nodul curent este mai mica decat cea mai mica
 * distanta de pana acum. (practic creez un disc/sfera/hipersfera in jurul
 * punctului cautat). Mai departe vom verifica daca vecinul opus frunzei de pe
 * care am venit are distanta in planul care imparte copacul mai mica decat
 * distanta de la punctul cautat la nodul curent (in termeni de mate, taiem cu
 * un plan domeniul, daca planul taie sfera, trecem pe celalat subarbore
 * intrucat mai pot exista noduri)
 * FOARTE IMPORTANT! Ca sa stim de pe ce ramura ne-am intors, initial m-am
 * gandit la doua variabile left si right trimise ca parametru, dar apoi mi-a
 * venit ideea de a codifica directia din care vin cu un semnal. Astfel, cand
 * ne intoarcem din recursivitate vom intoarce semnalul 1. Daca semnalul
 * ramane 1, atunci am venit de pe ramura dreapta, daca semnalul devine 2,
 * atunci am venit de pe ramura stanga. Si astfel vom sti pe ce ramura a
 * nodului sa mergem mai departe.
 */
int knn(kd_tree_t *tree, kd_node_t *cursor, int *v, int **coordinates,
		int *no_of_mins, unsigned int *min_dist, int curr_level);
/* Procedura ce verifica nodurile dintr-un kd-tree care se afla intr-un anumit
 * domeniu. Se vor verifica pe rand punctele. Daca coordonata corespunzatoare
 * nivelului curent este mai mica decat intervalul corespunzator coordonatei
 * nivelului curent, vom ignora subarborele stang (pentru mai mare vom ignora)
 * subarborele drept. In rest aborele se parcurge normal.
 */
void range_search(kd_tree_t *tree, kd_node_t *cursor, int *start, int *stop,
				  int **coordinates, int *no_of_points, int curr_level);
/* Procedura de sortare a unei matrici de coordonate, in care fiecare linie
 * coordinates[i] reprezinta coordonatele punctului i din matrice.
 * Punctele vor fi sortate initial dupa prima coordonata, apoi dupa a doua,
 * a treia, etc. (in cazul testelor noastre avem doar 3 dimensiuni, dar
 * procedura este generalizata pentru oricate)
 */
void sort_by_dim(kd_tree_t *tree, int **coordinates, int no_of_mins);
/* Procedura de eliberare a memoriei ocupata de un kd-tree. Se vor parcurge
 * pe rand toate nodurile, iar cand un nod nu mai are copii se vor elibera
 * atat nodul cat si vectorul de coordonate
 */
void free_kd_tree(kd_node_t *cursor);
