# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <time.h>
# include <math.h>

typedef struct {
unsigned long id; // Node identification
char *name; // Node name
double lat, lon; // Node geographical position
unsigned short nsucc; // Number of node successors; i. e. length of successors
unsigned *successors; //Stores the indices of successors
} node;

//Returns the distance between two coordinates using Haversine formula.
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
//Performs binary search in the id's of the node structure and stores the value of the index in a pointer. Returns true if the id is found or false otherwise.
bool binarysearch(unsigned long ident, node l[], unsigned n, unsigned *index)
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
// Handles errors and exits the program in case of one.
void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}
// Works as strtok() but returns an empty string (instead of NULL) when the field is empty.
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
// Calculates and prints the valence distribution togheter with the maximum and mean valence.
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
//Prints the nodes with at least one successor and non-empty name.
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