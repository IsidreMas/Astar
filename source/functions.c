# include "functions.h"

double get_distance(double lat1, double lon1, double lat2, double lon2)
{
    double const R = 6371e3;
    double phi1 = lat1 * M_PI/180;
    double phi2 = lat2 * M_PI/180;
    double delta_phi = (lat2 - lat1) * M_PI/180;
    double delta_lambda = (lon2 -lon1) * M_PI/180;
    double a = pow(sin(delta_phi/2),2) + cos(phi1)*cos(phi2)*pow(sin(delta_lambda/2), 2);
    
    return R * 2 * atan2(sqrt(a), sqrt(1-a));
}

unsigned long get_node(double lat, double lon, node *nodes, unsigned nnodes)
{
    unsigned long id;
    double test_distance, min_distance = INFINITY;
    for(int i=0; i<nnodes; i++)
        if((test_distance = get_distance(lat, lon, nodes[i].lat, nodes[i].lon))<min_distance && nodes[i].nsucc!=0)
        {
            min_distance = test_distance;
            id = nodes[i].id;
        }
    return id;
}


bool binarysearch(unsigned long ident, node *l, unsigned n, unsigned *index)
{
    int middle, start = 0, end = n-1;
    while (start <= end)
    {
      middle = (end + start)/2;
      if (l[middle].id == ident)
      {
          *index=middle;
          return true;
      }
      if (l[middle].id < ident)
         start = middle + 1;
      else
         end = middle - 1;
    }
    return false;
}

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}

char* strtoke(char *str, const char *delim)
{
  static char *start = NULL;
  char *token = NULL;
  if (str) start = str;
  if (!start) return NULL;
  token = start;
  start = strpbrk(start, delim);
  if (start) *start++ = '\0';
  return token;
}

void valence_distribution(unsigned short nsucc_max, unsigned nnodes, node nodes[])
{
    unsigned valence[nsucc_max], i;
    double mean_valence = 0;

    for(i=0U;i<=nsucc_max;i++)valence[i]=0UL;
    for(i=0U;i<nnodes;i++)valence[nodes[i].nsucc]++;
    for(i=0U;i<=nsucc_max;i++)
    {
        printf("Nodes with valence %u: %u\n",i, valence[i]);
        mean_valence += i*valence[i];
    }
    mean_valence/=nnodes;

    printf("\nThe graph has a maximum valence of %u and the mean valence is %f. \n", nsucc_max, mean_valence);
}

void print_nodes_sample(unsigned nnodes, node nodes[])
{   
    unsigned i;
    printf("Printing the nodes with at least one successors:\n");
    for(i=0U;i<nnodes;i++)
    {
        if(nodes[i].nsucc != 0 && strcmp(nodes[i].name, (char*)"")){
            printf("Id=%010ld Lat=%lf Long=%lf Number_of_successorss=%d Id_of_successor_1=%u Name=%s\n",nodes[i].id,nodes[i].lat,nodes[i].lon,nodes[i].nsucc,nodes[i].successors[0], nodes[i].name);
        }
    }
}

//Binary heap

Heap *CreateHeap(unsigned capacity){
    Heap *h = (Heap * ) malloc(sizeof(Heap)); //one is number of heap

    //check if memory allocation is fails
    if(h == NULL){
        printf("Memory Error!");
        exit(0);
    }
    h->count=0;
    h->capacity = capacity;
    h->distance = (double *) malloc(capacity*sizeof(double));
    h->node_index = (unsigned *) malloc(capacity*sizeof(unsigned));//size in bytes

    //check if allocation succeed
    if ( h->distance == NULL){
        printf("Memory Error!");
        exit(0);
    }
    if ( h->node_index == NULL){
        printf("Memory Error!");
        exit(0);
    }
    return h;
}

void insert(Heap *h, double distance, unsigned node_index){
    if( h->count < h->capacity){
        h->distance[h->count] = distance;
        h->node_index[h->count] = node_index;
        heapify_bottom_top(h, h->count);
        h->count++;
    }
}

void heapify_bottom_top(Heap *h,int index){
    double temp1, temp2;
    int parent_node = (index-1)/2;

    if(h->distance[parent_node] > h->distance[index]){
        //swap and recursive call
        temp1 = h->distance[parent_node];
        temp2 = h->node_index[parent_node];
        h->distance[parent_node] = h->distance[index];
        h->node_index[parent_node] = h->node_index[index];
        h->distance[index] = temp1;
        h->node_index[index] = temp2;
        heapify_bottom_top(h,parent_node);
    }
}

void heapify_top_bottom(Heap *h, int parent_node){
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min;
    double temp1, temp2;

    if(left >= h->count || left <0)
        left = -1;
    if(right >= h->count || right <0)
        right = -1;

    if(left != -1 && h->distance[left] < h->distance[parent_node])
        min=left;
    else
        min =parent_node;
    if(right != -1 && h->distance[right] < h->distance[min])
        min = right;

    if(min != parent_node){
        temp1 = h->distance[min];
        temp2 = h->node_index[min];
        h->distance[min] = h->distance[parent_node];
        h->node_index[min] = h->node_index[parent_node];
        h->distance[parent_node] = temp1;
        h->node_index[parent_node] = temp2;

        // recursive  call
        heapify_top_bottom(h, min);
    }
}

unsigned PopMin(Heap *h){
    unsigned pop;
    if(h->count==0){
        printf("\n__Heap is Empty__\n");
        return -1;
    }
    // replace first node by last and delete last
    pop = h->node_index[0];
    h->distance[0] = h->distance[h->count-1];
    h->node_index[0] = h->node_index[h->count -1];
    h->count--;
    heapify_top_bottom(h, 0);
    return pop;
}

void decreasePriority(Heap *h, double distance, unsigned node_index)
{
    int i;
    for (i = 0; i < h->count; i++)
        if (node_index == h->node_index[i])break;
    h->distance[i] = distance;

    if(i==0)return;
    if (h->distance[i]<h->distance[(i-1)/2]) heapify_bottom_top(h, i);
    else heapify_top_bottom(h, (i-1)/2);
}

void print(Heap *h){
    int i;
    printf("____________Print Heap_____________\n");
    for(i=0;i< h->count;i++){
        printf("-> %f ",h->distance[i]);
    }
    printf("->__/\\__\n");
}