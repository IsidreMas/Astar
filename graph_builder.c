# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>
# include <time.h>
# include <malloc.h>

//max numbers of char in one line of file 
# define MAXCHARSLINE 79857

typedef struct {
unsigned long id; // Node identification
char *name; // Node name
double lat, lon; // Node geographical position
unsigned short nsucc; // Number of node successors; i. e. length of successors
unsigned long *successors; // Node successors
} node;

// Performs a binary search.
//implicitly declares function 
//each nde has id (sorted) 
bool binarysearch(unsigned long ident, node l[],int n, unsigned long *index);

// Works as strtok() but returns an empty string (instead of NULL) when the field is empty.
// 
char* strtoke(char *str, const char *delim);

// Handles errors and exits the program in case of one.
void ExitError(const char *miss, int errcode);

//argc numb of arg, agrv argument vector 
int main(int argc, char *argv[]){
    FILE *nodesdata;
    node *nodes;
    clock_t local_time, global_time;
    unsigned long nnodes=0UL,i, field; //field is the column in the csv 
    char buffer[MAXCHARSLINE], *pch;
    double local_CPU_time, global_CPU_time;
    unsigned short nsucc_max = 0U;

    if(argc != 3)
    {
        printf("\nUsage: ./graph_builder 'PATH_TO_NODES_DATAFILE' 'NAME_OUTPUT_BINARY_FILE'\n");
        ExitError("Some arguments of the two expected were not given", 32);
    }
    
    printf("Opening %s\n\n", argv[1]);
    global_time=clock();
    nodesdata=fopen(argv[1],"r"); //opens the file with the nodes and ways 
    if(nodesdata==NULL)
    {
        ExitError("Couldn't acces the nodes data file", 32);
    }
    printf("Counting nodes...\n");
    // Counts the number of nodes
    local_time = clock();
    global_time = clock();

    //get first field of line 
    //compares string of first field to "node" 
    // counts the nodes (nnodes)
    while(fgets(buffer, sizeof(buffer), nodesdata))
    {
        pch = strtoke(buffer,"|");
        if (strcmp(pch, (char *)"node") == 0)nnodes++;
        else if(strcmp(pch, (char *)"way") == 0)break;
    }

    //rewind start at beginning of file again 
    rewind(nodesdata);
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("%lu nodes were found in %f CPU seconds.\n\n", nnodes, local_CPU_time);
    printf("Allocating memory for nodes and reading node lines...\n");
    local_time = clock();
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
            if(field == 2UL) //field 2 is the id in str, we save it to nodes id as an UL 
            {
                nodes[i].id = strtoul(pch,(char **)NULL, 10);
                if(i>0 && nodes[i-1].id>nodes[i].id)
                    ExitError("The nodes id's are not sorted in ascending order. That's necessary to perform binary search.", 32);
            }
            if(field == 3UL) //the name gets space and then copied to nodes 
            {   
                if((nodes[i].name = (char *) malloc(strlen(pch)*sizeof(char))) == NULL){
                    printf("size of allocation: %u\n", _msize(nodes[i].name));
                    ExitError("Couldn't allocate the memory for names", 32);

                }
                printf("size of allocation: %u at %lu\n", _msize(nodes[i].name), i);
                strcpy(nodes[i].name, pch);
            }
            if(field == 10UL)nodes[i].lat = atof(pch); //a tof converts string to double/float (stoes in long and lat )
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
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("The node lines were read in %f CPU seconds.\n\n", local_CPU_time);
    printf("Reading way lines and assigning node successors...\n");
    local_time = clock();
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
            if(first_node) //can become one while node 
            {
                previousnodeid = strtoul(pch,(char **)NULL, 10);
                if(!binarysearch(previousnodeid, nodes, nnodes, &previousnodeindex)) // if not found in binsearch go to next field 
                {
                    field++;
                    pch = strtoke(NULL, "|");
                    continue; //continue stops the while iteration, so we go back to beginning one field further 
                }
                first_node = false;
                field++;
                pch = strtoke(NULL, "|");
                continue;
            }
            nodeid = strtoul(pch,(char **)NULL, 10);
            if(!binarysearch(nodeid, nodes, nnodes, &nodeindex))
            {
        
                //first_node = true; // If A|B and B|C then A|C if B is missing. Uncomment to deactivate.
                field++; // if you dont find an id in the next field after previous field(id) go to next until you find one 
                pch = strtoke(NULL, "|");
                continue;
            }
            repeated_node=false;
            for(i=0;i<nodes[previousnodeindex].nsucc;i++)
            {
                if(nodes[previousnodeindex].successors[i]==nodeindex)repeated_node = true;//look if there is a succesor node already in list 
            }
            if(!repeated_node)
            {
                if((nodes[previousnodeindex].successors=(unsigned long *)realloc(nodes[previousnodeindex].successors, (nodes[previousnodeindex].nsucc + 1u)* sizeof(unsigned long)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                nodes[previousnodeindex].successors[nodes[previousnodeindex].nsucc] = nodeindex; //in succesor at place nsucc of previousnode the index of node gets written into
                nodes[previousnodeindex].nsucc++;
                if(nodes[previousnodeindex].nsucc > nsucc_max)nsucc_max=nodes[previousnodeindex].nsucc;
            }
            if(!oneway) // if not a one way the previous node is also a succ node for node 
            {
                repeated_node = false;
                for(i=0;i<nodes[nodeindex].nsucc;i++) // check for succesor duplicates also in node 
                {
                    if(nodes[nodeindex].successors[i]==previousnodeindex)repeated_node = true;
                }
                if(!repeated_node)
                {
                    if((nodes[nodeindex].successors=(unsigned long *)realloc(nodes[nodeindex].successors, (nodes[nodeindex].nsucc + 1u)* sizeof(unsigned long)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                    nodes[nodeindex].successors[nodes[nodeindex].nsucc]=previousnodeindex;
                    nodes[nodeindex].nsucc++;
                    if(nodes[nodeindex].nsucc > nsucc_max)nsucc_max=nodes[nodeindex].nsucc;
                }
            }
            previousnodeindex=nodeindex; // nodeindex will be the new node and the next node the node id 
            field++;
            pch = strtoke(NULL, "|");
        }
        fgets(buffer, sizeof(buffer), nodesdata);
        pch = strtoke(buffer,"|");
    }
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("The successors were read and assigned to nodes in %f CPU seconds.\n\n", local_CPU_time);

    fclose(nodesdata);

    printf("Writing the formatted graph...\n");
    local_time=clock();

    // Writes the binary file with the formatted graph, names and successors are writen in a single vector and need to be reasigned with pointers when reading.
    FILE *fin;

    // Computing the total number of successors
    unsigned long ntotnsucc=0UL;
    for(i=0UL; i < nnodes; i++) ntotnsucc += nodes[i].nsucc;
    // Computing the total number of characters in names
    unsigned long ntotnamechar=0UL;
    for(i=0UL; i < nnodes; i++) ntotnamechar += strlen(nodes[i].name)+1;
    // Setting the name of the binary file
    char *binary_file_name = argv[2];
    strcat(binary_file_name, ".bin");
    if ((fin = fopen (binary_file_name, "wb")) == NULL)
        ExitError("the output binary data file cannot be opened", 31);

    // Global data −−− header 
    // number nodes, total number of succeors, total number of charectars on name of nodes 
    if( fwrite(&nnodes, sizeof(unsigned long), 1, fin) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, fin) + fwrite(&ntotnamechar, sizeof(unsigned long), 1, fin)!= 3 )
        ExitError("when initializing the output binary data file", 32);
    // Writing all nodes 
    if( fwrite(nodes, sizeof(node), nnodes, fin) != nnodes )
        ExitError("when writing nodes to the output binary data file", 32);
    // Writing sucessors in blocks 
    for(i=0UL; i < nnodes; i++)if(nodes[i].nsucc)if( fwrite(nodes[i].successors, sizeof(unsigned long), nodes[i].nsucc, fin) != nodes[i].nsucc )
        ExitError("when writing edges to the output binary data file", 32);
    // Writing names in blocks
    for(i=0UL; i < nnodes; i++)if( fwrite(nodes[i].name, sizeof(char), strlen(nodes[i].name)+1, fin) != strlen(nodes[i].name)+1)
        ExitError("when writing edges to the output binary data file", 32);

    fclose(fin);

    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    global_CPU_time = (double)(clock()-global_time)/CLOCKS_PER_SEC;
    printf("The graph was written into %s in %f CPU seconds. You can now execute the program './Astar %s'\n\n", binary_file_name, local_CPU_time, binary_file_name);

    unsigned long valence[nsucc_max];
    double mean_valence = 0;

    for(i=0UL;i<=nsucc_max;i++)valence[i]=0UL;
    for(i=0;i<nnodes;i++)valence[nodes[i].nsucc]++;
    for(i=0UL;i<=nsucc_max;i++)
    {
        printf("Nodes with valence %lu: %lu\n",i, valence[i]);
        mean_valence += i*valence[i];
    }
    mean_valence/=nnodes;

    printf("\nThe graph has a maximum valence of %u and the mean valence is %f. \n", nsucc_max, mean_valence);
    printf("The total computation time to build and write the graph with summary data was %f CPU seconds.\n\n", global_CPU_time);
    printf("Freeing allocated memory...\n");
    //free: the pointer to a memory block previously allocated with malloc, calloc or realloc to be deallocated
    //so after writing file we de allocate memory  
    for(i=0UL;i<nnodes;i++)
    {
        free(nodes[i].name);
        free(nodes[i].successors);
    }
    free(nodes);

    printf("You can now execute the program './Astar %s'\n\n", argv[2]);

    return 0;
}

bool binarysearch(unsigned long ident, node l[],int n, unsigned long *index)// gives the index of id of node without returning  
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

//if you put NULL you go to the next field of the buffer (next field indicated by delimier)
//NuLL remembers that you called the function 
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
// Questions: what is an unsigned long? so you can store a vector in an UL? how does c know its a vector? 
// header: do you also write th header when it occurs in if clause 
// why do you need the first if  (writing succesors) 
