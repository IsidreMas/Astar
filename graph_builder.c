# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>

# define MAXCHARSLINE 79857

typedef struct {
unsigned long id; // Node identification
char *name; // Node name
double lat, lon; // Node geographical position
unsigned short nsucc; // Number of node successors; i. e. length of successors
unsigned long *successors; // Node successors
} node;

// Performs a binary search.
bool binarysearch(unsigned long ident, node l[],int n, unsigned long *index);

// Works as strtok() but returns an empty string (instead of NULL) when the field is empty.
char* strtoke(char *str, const char *delim);

// Handles errors and exits the program in case of one.
void ExitError(const char *miss, int errcode);

int main(int argc, char *argv[]){
    FILE *nodesdata;
    node *nodes;
    unsigned long nnodes=0UL,i, field;
    char buffer[MAXCHARSLINE], *pch;
    
    nodesdata=fopen("maps_data/cataluna.csv","r");
    if(nodesdata==NULL)
    {
        ExitError("Couldn't acces the nodes data file", 32);
    }
    
    // Counts the number of nodes
    while(fgets(buffer, sizeof(buffer), nodesdata))
    {
        pch = strtoke(buffer,"|");
        if (strcmp(pch, (char *)"node") == 0)nnodes++;
        else if(strcmp(pch, (char *)"way") == 0)break;
    }
    rewind(nodesdata);

    // Reserves the memory for the nodes.
    if((nodes=(node *)malloc(nnodes*sizeof(node)))==NULL)
        ExitError("Couldn't allocate the memory for nodes", 32);

    // Skips the first lines that are not nodes.
    do
    {
        fgets(buffer, sizeof(buffer), nodesdata); 
        pch = strtoke(buffer,"|");
    }while(strcmp(pch, (char *)"node") != 0);

    // Reads the data in the nodes lines and store it in the vector nodes.
    for(i=0UL;i<nnodes;i++)
    {
        field = 1UL;
        while (pch != NULL)
        {
            if(field == 2UL)nodes[i].id = strtoul(pch,(char **)NULL, 10);
            if(field == 3UL)
            {   
                if((nodes[i].name = (char *) malloc(strlen(pch)*sizeof(char))) == NULL)
                    ExitError("Couldn't allocate the memory for names", 32);
                strcpy(nodes[i].name, pch);
            }
            if(field == 10UL)nodes[i].lat = atof(pch);
            if(field == 11UL)nodes[i].lon = atof(pch);
            field++;
            pch = strtoke(NULL, "|");
        }
        nodes[i].nsucc=0U;
        if((nodes[i].successors=(unsigned long *)malloc(nodes[i].nsucc*sizeof(unsigned long)))==NULL)
            ExitError("Couldn't allocate the memory for successors", 32);

        fgets(buffer, sizeof(buffer), nodesdata);
        pch = strtoke(buffer,"|");
    }

    // Reads the consecutive nodes of the way lines and keep two in memory: previousnodeindex and nodeindex.
    unsigned long nodeid, previousnodeid, nodeindex, previousnodeindex;
    bool first_node, repeated_node, oneway; //This are flags to deal with the inconsistencies in the file and reproduce the successors in twoway roads.

    while(strcmp(pch, (char *)"way") == 0)
    {
        first_node = true;
        field = 1UL;
        oneway = false;
        // Skips the first fields before the node members of the way and sets the flag oneway if it's true.
        while(pch!= NULL && field < 10UL)
        {
            if(field == 8UL)if(strcmp(pch, (char *)"oneway")==0)oneway = true;
            field++;
            pch = strtoke(NULL, "|");
        }
        // Deals with the inconsistencies of the file and properly stores the successors of each node.
        while (pch != NULL)
        {     
            if(first_node)
            {
                previousnodeid = strtoul(pch,(char **)NULL, 10);
                if(!binarysearch(previousnodeid, nodes, nnodes, &previousnodeindex))
                {
                    field++;
                    pch = strtoke(NULL, "|");
                    continue;
                }
                first_node = false;
                field++;
                pch = strtoke(NULL, "|");
                continue;
            }
            nodeid = strtoul(pch,(char **)NULL, 10);
            if(!binarysearch(nodeid, nodes, nnodes, &nodeindex))
            {
                first_node = true;
                field++;
                pch = strtoke(NULL, "|");
                continue;
            }
            repeated_node=false;
            for(i=0;i<nodes[previousnodeindex].nsucc;i++)
            {
                if(nodes[previousnodeindex].successors[i]==nodeindex)repeated_node = true;
            }
            if(!repeated_node)
            {
                if((nodes[previousnodeindex].successors=(unsigned long *)realloc(nodes[previousnodeindex].successors, (nodes[previousnodeindex].nsucc + 1u)* sizeof(unsigned long)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                nodes[previousnodeindex].successors[nodes[previousnodeindex].nsucc] = nodeindex;
                nodes[previousnodeindex].nsucc++;
            }
            if(!oneway)
            {
                repeated_node = false;
                for(i=0;i<nodes[nodeindex].nsucc;i++)
                {
                    if(nodes[nodeindex].successors[i]==previousnodeindex)repeated_node = true;
                }
                if(!repeated_node)
                {
                    if((nodes[nodeindex].successors=(unsigned long *)realloc(nodes[nodeindex].successors, (nodes[nodeindex].nsucc + 1u)* sizeof(unsigned long)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                    nodes[nodeindex].successors[nodes[nodeindex].nsucc]=previousnodeindex;
                    nodes[nodeindex].nsucc++;
                }
            }
            previousnodeindex=nodeindex;
            field++;
            pch = strtoke(NULL, "|");
        }
        fgets(buffer, sizeof(buffer), nodesdata);
        pch = strtoke(buffer,"|");
    }

    fclose(nodesdata);

    // Error checker (This can be commented out)
    printf("Checking if there were errors reading the file...\n");
    for(i=1;i<nnodes;i++){
        if(nodes[i-1].id>nodes[i].id)
            ExitError("The nodes id's are not sorted in ascending order. That's necessary to perform binary search.", 32);//This checks that the ID's are sorted (necessary to perform binary search)
    }
    
    // We print the nodes with at least one successor to control if it works
    printf("Printing the nodes with at least one successors:\n");
    for(i=0;i<nnodes;i++){
        if(nodes[i].nsucc != 0U && strcmp(nodes[i].name, (char*)"")){
            printf("Id=%010ld Lat=%lf Long=%lf Number_of_successorss=%d Id_of_successor_1=%lu Name=%s\n",nodes[i].id,nodes[i].lat,nodes[i].lon,nodes[i].nsucc,nodes[i].successors[0], nodes[i].name);
        }
    }

    // Writes the binary file with the formatted graph, names and successors are writen in a single vector and need to be reasigned with pointers when reading.
    FILE *fin;

    /* Computing the total number of successors */
    unsigned long ntotnsucc=0UL;
    for(i=0UL; i < nnodes; i++) ntotnsucc += nodes[i].nsucc;
    /* Computing the total number of characters in names */
    unsigned long ntotnamechar=0UL;
    for(i=0UL; i < nnodes; i++) ntotnamechar += strlen(nodes[i].name)+1;
    /* Setting the name of the binary file */
    if ((fin = fopen ("Graph.bin", "wb")) == NULL)
        ExitError("the output binary data file cannot be opened", 31);
    /* Global data −−− header */
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) + fwrite(&ntotnamechar, sizeof(unsigned long), 1, fin)!= 3 )
        ExitError("when initializing the output binary data file", 32);
    /* Writing all nodes */
    if( fwrite(nodes, sizeof(node), nnodes, fin) != nnodes )
        ExitError("when writing nodes to the output binary data file", 32);
    /* Writing sucessors in blocks */
    for(i=0UL; i < nnodes; i++)if(nodes[i].nsucc)if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin) != nodes[i].nsucc )
        ExitError("when writing edges to the output binary data file", 32);
    /* Writing names in blocks */
    for(i=0UL; i < nnodes; i++)if( fwrite(nodes[i].name, sizeof(char), strlen(nodes[i].name)+1, fin) != strlen(nodes[i].name)+1)
        ExitError("when writing edges to the output binary data file", 32);

    fclose(fin);
    free(nodes);

    return 0;
}

bool binarysearch(unsigned long ident, node l[],int n, unsigned long *index)
{
    unsigned long middle, start = 0, end = n-1;
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

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}
