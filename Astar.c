# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define MAXSUCCESSORS 9

typedef struct{
    char *wayname; // Street Id (could be the name)
    unsigned long nodeposition; // Position in nodes vector of the destination of the successors
}successorsinfo;

typedef struct {
unsigned long id; // Node identification
char *name;
double lat, lon; // Node position
unsigned short nsucc; // Number of node successors; i. e. length of successors
successorsinfo successors[MAXSUCCESSORS];
} node;

void ExitError(const char *miss, int errcode) {
fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss); exit(errcode);
}

int main (int argc, char *argv[])
{
    FILE *fin;
    unsigned long nnodes, ntotnsucc=0UL, i;
    node *nodes;

    if ((fin = fopen ("Graph.bin", "rb")) == NULL)
    ExitError("the data file does not exist or cannot be opened", 11);
    /* Global data −−− header */
    if( fread(&nnodes, sizeof(unsigned long), 1, fin) + fread(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
    ExitError("when reading the header of the binary data file", 12);
    /* getting memory for all data */
    if((nodes=(node *)malloc(nnodes*sizeof(node)))==NULL)
    ExitError("when allocating memory for the nodes vector", 13);
    /*if( (
    allsuccessors = (unsigned long *) malloc(ntotnsucc*sizeof(unsigned long))
    ) == NULL)
    ExitError("when allocating memory for the edges vector", 15);
    /* Reading all data from file */
    if( fread(nodes, sizeof(node), nnodes, fin) != nnodes )
    ExitError("when reading nodes from the binary data file", 17);
    /* if(fread(allsuccessors, sizeof(unsigned long), ntotnsucc, fin) != ntotnsucc)
    ExitError("when reading sucessors from the binary data file", 18);*/
    fclose(fin);
    /* Setting pointers to successors */
    /*for(i=0; i < nnodes; i++) if(nodes[i].nsucc) {
    nodes[i].successors = allsuccessors; allsuccessors += nodes[i].nsucc;
    }*/

    printf("%lu", nnodes);

    printf("Printing the nodes with at least one successors:\n");
    for(i=0;i<nnodes;i++){
        if(nodes[i].nsucc != 0){
            printf("Id=%010ld Lat=%lf Long=%lf Number_of_successorss=%d Id_of_successor_1=%lu Street=%s\n",nodes[i].id,nodes[i].lat,nodes[i].lon,nodes[i].nsucc,nodes[i].successors[nodes[i].nsucc-1].nodeposition, nodes[i].successors[nodes[i].nsucc -1].wayname);
        }
    }

    return 0;
}