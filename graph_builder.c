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

unsigned long binarysearch(unsigned long ident, node l[],int n, unsigned long max_id); // Performs a binary search.

char* strtoke(char *str, const char *delim); // Works as strtok() but returns an empty string (instead of NULL) when the field is empty.

void ExitError(const char *miss, int errcode); // Handles errors and exits the program in case of one.

int main(int argc, char *argv[]){
    FILE *nodesdata;
    node *nodes;
    unsigned long nnodes=0,i,buffer_length, field;
    char buffer[79858], *pch;
    
    nodesdata=fopen("maps_data/cataluna.csv","r");
    if(nodesdata==NULL)
    {
        printf("\nWe cannot acces to the nodes data file\n");
        return 1;
    }
    // We count the number of nodes

    while(fgets(buffer, sizeof(buffer), nodesdata))
    {
        pch = strtoke(buffer,"|");
        if (strcmp(pch, (char *)"node") == 0) {
            nnodes++;
        }
    }

    rewind(nodesdata);
    // We reserve the memory for the nodes
    if((nodes=(node *)malloc(nnodes*sizeof(node)))==NULL)
    {
        printf("Error in the memory allocation of nodes.\n");
        return 2;
    }

    do
    {
        fgets(buffer, sizeof(buffer), nodesdata); 
        pch = strtoke(buffer,"|");
    }while(strcmp(pch, (char *)"node") != 0);

    // We read the data in the nodes file and store it in the vector nodes.
    for(i=0;i<nnodes;i++)
    {
        field = 1;
        while (pch != NULL)
        {
            if(field == 2)nodes[i].id = strtoul(pch,(char **)NULL, 10);
            if(field == 3)nodes[i].name = pch;
            if(field == 10)nodes[i].lat = atof(pch);
            if(field == 11)nodes[i].lon = atof(pch);
            field++;
            pch = strtoke(NULL, "|");
        }
        nodes[i].nsucc=0;
        fgets(buffer, sizeof(buffer), nodesdata);
        if(i==nnodes-1)buffer_length=strlen(buffer);
        pch = strtoke(buffer,"|");
    }

    fseek(nodesdata, -buffer_length, SEEK_CUR);

    // Now we have to read the nodes of the street and keep two in memory: previousnodeid and nodeid.
    unsigned long nodeid, previousnodeid, wayid, max_id = nodes[nnodes-1].id+1, previousnode, previousnodeposition;
    short flag, oneway;
    char *wayname;

    while(strcmp(pch, (char *)"way") == 0)
    {
        flag = 1;
        field = 1;
        oneway = 0;
        while (pch != NULL)
        {
            if(field == 2)wayid = strtoul(pch,(char **)NULL, 10);
            if(field == 3)wayname = pch;
            if(field == 8)if(strcmp(pch, (char *)"oneway")==0)oneway = 1;
            if(field >= 10)
            {   
                if(flag==1)
                {
                    flag = 0;
                    previousnodeid = strtoul(pch,(char **)NULL, 10);
                    if((previousnodeposition = binarysearch(previousnodeid, nodes, nnodes, max_id)) == max_id)
                    {
                        flag = 1;
                        field++;
                        pch = strtoke(NULL, "|");
                        continue;
                    }
                    field++;
                    pch = strtoke(NULL, "|");
                    if (pch == NULL)break;// ways with just one node are discarded automatically because field=11 is NULL
                }
                nodeid = strtoul(pch,(char **)NULL, 10);
                if((previousnode = binarysearch(nodeid, nodes, nnodes, max_id)) == max_id)
                {
                    flag = 1;
                    field++;
                    pch = strtoke(NULL, "|");
                    continue;
                }

                if(flag == 0)
                {
                    for(i=0;i<nodes[previousnodeposition].nsucc;i++)
                    {
                        if(nodes[previousnodeposition].successors[i].nodeposition==previousnode)flag = 1;
                    }
                    if(flag == 0)
                    {
                        nodes[previousnodeposition].successors[nodes[previousnodeposition].nsucc].nodeposition = previousnode;
                        nodes[previousnodeposition].successors[nodes[previousnodeposition].nsucc].wayname = wayname;
                        nodes[previousnodeposition].nsucc++;
                    }
                    if(oneway == 0)
                    {
                        for(i=0;i<nodes[previousnode].nsucc;i++)
                        {
                            if(nodes[previousnode].successors[i].nodeposition==previousnodeposition)flag = 1;
                        }
                        if(flag == 0)
                        {
                            nodes[previousnode].successors[nodes[previousnode].nsucc].nodeposition=previousnodeposition;
                            nodes[previousnode].successors[nodes[previousnode].nsucc].wayname = wayname;
                            nodes[previousnode].nsucc++;
                        }
                    }
                    previousnodeposition=previousnode;
                    flag = 0;
                }

            }
            field++;
            pch = strtoke(NULL, "|");
        }
        fgets(buffer, sizeof(buffer), nodesdata);
        pch = strtoke(buffer,"|");
    }

    fclose(nodesdata);
    // Let's show the result
    printf("Printing the result of reading the nodes file:\n");
    for(i=1;i<nnodes;i++){
        if(nodes[i-1].id>nodes[i].id)printf("FAIL");

        //printf("Id=%010ld Lat=%lf Long=%lf\n",nodes[i].id,nodes[i].lat,nodes[i].lon);
    }
    
    // We print the nodes with at least one successors to control if it works
    printf("Printing the nodes with at least one successors:\n");
    for(i=0;i<nnodes;i++){
        if(nodes[i].nsucc != 0){
            //printf("Id=%010ld Lat=%lf Long=%lf Number_of_successorss=%d Id_of_successor_1=%lu Name=%s\n",nodes[i].id,nodes[i].lat,nodes[i].lon,nodes[i].nsucc,nodes[i].successors[nodes[i].nsucc-1].nodeposition, nodes[i].name);
        }
    }

    // Writing the binary file to be used in the Astar.c program

    FILE *fin;
    // Computing the total number of successors
    unsigned long ntotnsucc=0UL;

    for(i=0; i < nnodes; i++) ntotnsucc += nodes[i].nsucc;

    if ((fin = fopen ("Graph.bin", "wb")) == NULL)
    ExitError("the output binary data file cannot be opened", 31);
    printf("%lu", nnodes);
    //Global data −−− header
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) != 2 )
    ExitError("when initializing the output binary data file", 32);

    //Writing all nodes
    if( fwrite(nodes, sizeof(node), nnodes, fin) != nnodes )
    ExitError("when writing nodes to the output binary data file", 32);

    /*//Writing sucessors in blocks
    for(i=0; i < nnodes; i++) if(nodes[i].nsucc)
    {
        if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin) !=
        nodes[i].nsucc )
        ExitError("when writing edges to the output binary data file", 32);
    }*/
    fclose(fin);

    return 0;
}

unsigned long binarysearch(unsigned long ident, node l[],int n, unsigned long max_id)
{
    unsigned long middle, start = 0, end = n-1;
    while (start <= end){
      middle = start + (end- start)/2;
      if (l[middle].id == ident)
         return middle;
      if (l[middle].id < ident)
         start = middle + 1;
      else
         end = middle - 1;
   }
   return max_id;
}

char* strtoke(char *str, const char *delim)
{
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str) start = str;
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
}

void ExitError(const char *miss, int errcode)
{
    fprintf (stderr, "\nERROR: %s.\nStopping...\n\n", miss);
    exit(errcode);
}