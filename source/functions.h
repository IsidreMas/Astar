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

/*
*Returns the distance between two coordinates using Haversine formula.
*/
double get_distance(double lat1, double lon1, double lat2, double lon2);
/*
*Loops through all the nodes to find the closest node with at least one successor to the given coordinates.
*
*Parameters:
*-lat: Latitude.
*-lon: Longitude.
*-nodes[]: Structure containing all the nodes.
*-nnodes: Total number of nodes.
*/
unsigned long get_node(double lat, double lon, node *nodes, unsigned nnodes);
/*
*Performs binary search in the id's of the node structure and stores the value of the index in a pointer. Returns true if the id is found or false otherwise.
*/
bool binarysearch(unsigned long ident, node *nodes, unsigned n, unsigned *index);
/*
*Handles errors and exits the program in case of one.
*/
void ExitError(const char *miss, int errcode);
/*
*Works as strtok() but returns an empty string (instead of NULL) when the field is empty.
*/
char* strtoke(char *str, const char *delim);
/*
*Calculates and prints the valence distribution togheter with the maximum and mean valence.
*/
void valence_distribution(unsigned short nsucc_max, unsigned nnodes, node nodes[]);
/*
*Prints the nodes with at least one successor and non-empty name.
*/
void print_nodes_sample(unsigned nnodes, node nodes[]);

/*
* Heap structure
*/
struct Heap{
    double *distance;
    unsigned *node_index;
    int count;
    int capacity;
};
typedef struct Heap Heap;

/*
* Returns an empty heap structure with a given capacity.
*/
Heap *CreateHeap(unsigned capacity);
/*
*Inserts an index to the binary heap taking distance as priority value.
*/
void insert(Heap *h, double distance, unsigned node_index);
/*
*Prints the heap for testing purposes.
*/
void print(Heap *h);
/*
*Heapify operation bottom-top.
*/
void heapify_bottom_top(Heap *h,int index);
/*
*Heapify operation top-bottom.
*/
void heapify_top_bottom(Heap *h, int parent_node);
/*
*Returns the index with higher priority and pops it from the heap.
*/
unsigned PopMin(Heap *h);
/*
*Updates the priority of a node_index already in the heap.
*/
void decreasePriority(Heap *h, double distance, unsigned node_index);