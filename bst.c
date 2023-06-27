#include "bst.h"

kd_node_t *kd_create_node(kd_tree_t *tree)
{
	kd_node_t *new_node = calloc(1, sizeof(kd_node_t));
	new_node->right = NULL;
	new_node->left = NULL;
	new_node->dims = calloc(tree->no_of_dims, sizeof(int));
	return new_node;
}

kd_tree_t *kd_create(int no_of_dims)
{
	kd_tree_t *new_tree = calloc(1, sizeof(kd_tree_t));
	new_tree->no_of_dims = no_of_dims;
	new_tree->root = NULL;
	return new_tree;
}

void kd_tree_insert(kd_tree_t *tree, kd_node_t *new_node, int curr_level)
{
	if (!tree->root) {
		tree->root = new_node;
		return;
	}
	kd_node_t *cursor = tree->root;
	while (cursor) {
		if (new_node->dims[curr_level % tree->no_of_dims] >=
			cursor->dims[curr_level % tree->no_of_dims]) {
			if (!cursor->right) {
				cursor->right = new_node;
				return;
			}
			cursor = cursor->right;
		} else {
			if (!cursor->left) {
				cursor->left = new_node;
				return;
			}
			cursor = cursor->left;
		}
		curr_level++;
	}
}

void load_data(kd_tree_t **tree, FILE *fin, int *n)
{
	int no_of_dims, x;
	fscanf(fin, "%d %d", n, &no_of_dims);
	(*tree) = kd_create(no_of_dims);
	for (int i = 0; i < (*n); i++) {
		kd_node_t *new_node = kd_create_node(*tree);
		for (int j = 0; j < no_of_dims; j++) {
			fscanf(fin, "%d", &x);
			new_node->dims[j] = x;
		}
		kd_tree_insert(*tree, new_node, 0);
	}
}

int knn(kd_tree_t *tree, kd_node_t *cursor, int *v, int **coordinates,
		int *no_of_mins, unsigned int *min_dist, int curr_level)
{
	if (!cursor)
		return 1;
	int sig = 0, height = curr_level % tree->no_of_dims;
	if (v[height] >= cursor->dims[height])
		sig = knn(tree, cursor->right, v, coordinates, no_of_mins, min_dist,
				  curr_level + 1);
	else
		sig = knn(tree, cursor->left, v, coordinates, no_of_mins, min_dist,
				  curr_level + 1) + 1;
	if (sig) {
		unsigned int curr_dist = 0;
		for (int i = 0; i < tree->no_of_dims; i++)
			curr_dist += (v[i] - cursor->dims[i]) * (v[i] - cursor->dims[i]);
		if (curr_dist < (*min_dist)) {
			(*no_of_mins) = 0;
			(*min_dist) = curr_dist;
			for (int i = 0; i < tree->no_of_dims; i++)
				coordinates[*no_of_mins][i] = cursor->dims[i];
			(*no_of_mins)++;
		} else if (curr_dist == (*min_dist)) {
			(*min_dist) = curr_dist;
			for (int i = 0; i < tree->no_of_dims; i++)
				coordinates[*no_of_mins][i] = cursor->dims[i];
			(*no_of_mins)++;
		}
	}
	unsigned int sphere_plane_distance = (v[height] - cursor->dims[height]) *
								(v[height] - cursor->dims[height]);
	if (sphere_plane_distance < (*min_dist)) {
		if (sig == 1) {
			knn(tree, cursor->left, v, coordinates, no_of_mins, min_dist,
				curr_level + 1);
		} else if (sig == 2) {
			knn(tree, cursor->right, v, coordinates, no_of_mins, min_dist,
				curr_level + 1);
		}
	}
	return 1;
}

void range_search(kd_tree_t *tree, kd_node_t *cursor, int *start, int *stop,
				  int **coordinates, int *no_of_points, int curr_level)
{
	if (!cursor)
		return;
	int ok = 0, right = 1, left = 1;
	for (int i = 0; i < tree->no_of_dims; i++)
		if (cursor->dims[i] > stop[i]) {
			if (i == (curr_level % tree->no_of_dims))
				right = 0;
			ok = 1;
		} else if (cursor->dims[i] < start[i]) {
			if (i == (curr_level % tree->no_of_dims))
				left = 0;
			ok = 1;
		}
	if (!ok) {
		for (int i = 0; i < tree->no_of_dims; i++)
			coordinates[*no_of_points][i] = cursor->dims[i];
		(*no_of_points)++;
	}
	if (!left) {
		range_search(tree, cursor->right, start, stop, coordinates,
					 no_of_points, curr_level + 1);
	} else if (!right) {
		range_search(tree, cursor->left, start, stop, coordinates,
					 no_of_points, curr_level + 1);
	} else {
		range_search(tree, cursor->left, start, stop, coordinates,
					 no_of_points, curr_level + 1);
		range_search(tree, cursor->right, start, stop, coordinates,
					 no_of_points, curr_level + 1);
	}
}

void sort_by_dim(kd_tree_t *tree, int **coordinates, int no_of_points)
{
	int aux;
	for (int i = 0; i < no_of_points - 1; i++)
		for (int j = 0; j < tree->no_of_dims; j++)
			if (coordinates[i][j] > coordinates[i + 1][j]) {
				for (int k = 0; k < tree->no_of_dims; k++) {
					aux = coordinates[i][k];
					coordinates[i][k] = coordinates[i + 1][k];
					coordinates[i + 1][k] = aux;
				}
				break;
			} else if (coordinates[i][j] < coordinates[i + 1][j]) {
				break;
			}
	for (int i = 0; i < no_of_points; i++) {
		for (int j = 0; j < tree->no_of_dims; j++)
			printf("%d ", coordinates[i][j]);
		printf("\n");
	}
}

void free_kd_tree(kd_node_t *cursor)
{
	if (!cursor)
		return;
	free_kd_tree(cursor->left);
	cursor->left = NULL;
	free_kd_tree(cursor->right);
	cursor->right = NULL;
	if (!cursor->right && !cursor->left) {
		free(cursor->dims);
		free(cursor);
		cursor = NULL;
		return;
	}
}
