# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <time.h>
# include <math.h>

typedef struct node{
    unsigned long id; // Node identification
    char *name; // Node name
    double lat, lon; // Node geographical position
    unsigned short nsucc; // Number of node successors; i. e. length of successors
    unsigned *successors; //Stores the indices of successors
}node;

double get_distance(double lat1, double lon1, double lat2, double lon2);
unsigned long get_node(double lat, double lon, node nodes[], unsigned nnodes);
bool binarysearch(unsigned long ident, node l[], unsigned n, unsigned *index);
void ExitError(const char *miss, int errcode);
char* strtoke(char *str, const char *delim);
void valence_distribution(unsigned short nsucc_max, unsigned nnodes, node nodes[]);
void print_nodes_sample(unsigned nnodes, node nodes[]);

struct Heap{
    double *distance;
    unsigned *node_index;
    int count;
    int capacity;
};
typedef struct Heap Heap;

Heap *CreateHeap(unsigned capacity);
void insert(Heap *h, double distance, unsigned node_index);
void print(Heap *h);
void heapify_bottom_top(Heap *h,int index);
void heapify_top_bottom(Heap *h, int parent_node);
unsigned PopMin(Heap *h);
void decreasePriority(Heap *h, double distance, unsigned node_index);