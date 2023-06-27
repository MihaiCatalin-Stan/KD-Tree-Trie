#include "bst.h"

int main(void)
{
	char *op = calloc(MAX_LEN, sizeof(char)),
	*word = calloc(MAX_LEN, sizeof(char));
	int *v, **coordinates, n, no_of_mins, *start, *stop;
	unsigned int min_dist = INF;
	kd_tree_t *tree = NULL;
	while (1) {
		scanf("%s", op);
		if (!strcmp(op, "LOAD")) {
			scanf("%s", word);
			FILE *fin = fopen(word, "r");
			load_data(&tree, fin, &n);
			fclose(fin);
			v = calloc(tree->no_of_dims, sizeof(int));
			coordinates = calloc(n, sizeof(int *));
			for (int i = 0; i < n; i++)
				coordinates[i] = calloc(tree->no_of_dims, sizeof(int));
			start = calloc(tree->no_of_dims, sizeof(int));
			stop = calloc(tree->no_of_dims, sizeof(int));
		} else if (!strcmp(op, "NN")) {
			for (int i = 0; i < tree->no_of_dims; i++)
				scanf("%d", &v[i]);
			min_dist = INF;
			no_of_mins = 0;
			knn(tree, tree->root, v, coordinates, &no_of_mins, &min_dist, 0);
			sort_by_dim(tree, coordinates, no_of_mins);
		} else if (!strcmp(op, "RS")) {
			for (int i = 0; i < tree->no_of_dims; i++)
				scanf("%d %d ", &start[i], &stop[i]);
			no_of_mins = 0;
			range_search(tree, tree->root, start, stop, coordinates,
						 &no_of_mins, 0);
			sort_by_dim(tree, coordinates, no_of_mins);
		} else if (!strcmp(op, "EXIT")) {
			for (int i = 0; i < n; i++)
				free(coordinates[i]);
			free(coordinates);
			free(v);
			free(start);
			free(stop);
			free(op);
			free(word);
			free_kd_tree(tree->root);
			free(tree);
			exit(0);
		}
	}
}
