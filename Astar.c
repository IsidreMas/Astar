# include "functions.h"

int main (int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("\nUsage: ./graph_builder 'GRAPH_BINARY_FILE'\n");
        ExitError("The argument expected was not given", 32);
    }

    FILE *binary_file;
    unsigned long ntotnsucc, ntotnamechar;
    unsigned *allsuccessors, i, nnodes;
    char *allnames;
    node *nodes;

    if ((binary_file = fopen (argv[1], "rb")) == NULL)
        ExitError("the data file does not exist or cannot be opened", 11);
    /* Global data −−− header */
    if( fread(&nnodes, sizeof(unsigned), 1, binary_file) + fread(&ntotnsucc, sizeof(unsigned long), 1, binary_file) + fread(&ntotnamechar, sizeof(unsigned long), 1, binary_file) != 3 )
        ExitError("when reading the header of the binary data file", 12);
    /* getting memory for all data */
    if((nodes = (node *) malloc(nnodes*sizeof(node))) == NULL)
        ExitError("when allocating memory for the nodes vector", 13);
    if((allsuccessors = (unsigned *) malloc(ntotnsucc*sizeof(unsigned)) ) == NULL)
        ExitError("when allocating memory for the edges vector", 15);
    if((allnames = (char *) malloc(ntotnamechar*sizeof(char)) ) == NULL)
        ExitError("when allocating memory for the edges vector", 15);
    /* Reading all data from file */
    if( fread(nodes, sizeof(node), nnodes, binary_file) != nnodes )
        ExitError("when reading nodes from the binary data file", 17);
    if(fread(allsuccessors, sizeof(unsigned), ntotnsucc, binary_file) != ntotnsucc)
        ExitError("when reading sucessors from the binary data file", 18);
    if(fread(allnames, sizeof(char), ntotnamechar, binary_file) != ntotnamechar)
        ExitError("when reading names from the binary data file", 18);
    fclose(binary_file);
    /* Setting pointers to successors and names*/
    for(i=0UL; i < nnodes; i++)
    {
        if(nodes[i].nsucc)
        {
            nodes[i].successors = allsuccessors;
            allsuccessors += nodes[i].nsucc;
        }
        nodes[i].name = allnames;
        allnames += strlen(allnames)+1;
    }

    unsigned long id_origin, id_destination;
    unsigned index_origin, index_destination;

    printf("\nThis program will perform A* algorithm to find the shortest length route between two nodes in %s\n\n",argv[1]);
    printf("Introduce the id of the origin:");
    scanf("%lu", &id_origin);
    while(!binarysearch(id_origin, nodes, nnodes, &index_origin))
    {
        printf("\nThe introduced id doesn't exist in the graph, try again:");
        scanf("%lu", &id_origin);
    }
    printf("Introduce the id of the destination:");
    scanf("%lu", &id_destination);
    while(!binarysearch(id_destination, nodes, nnodes, &index_destination))
    {
        printf("\nThe introduced id doesn't exist in the graph, try again:");
        scanf("%lu", &id_destination);
    }

    if(strcmp(nodes[index_origin].name, (char*)"")!=0 && strcmp(nodes[index_destination].name, (char*)"")!=0)
    printf("\nDo you want to perform A* algorithm from %s to %s? (Y/N)", nodes[index_origin].name, nodes[index_destination].name);
    else if(strcmp(nodes[index_origin].name, (char*)"")==0 && strcmp(nodes[index_destination].name, (char*)"")!=0)
    printf("\nDo you want to perform A* algorithm from node with id %lu to %s? (Y/N)", nodes[index_origin].id, nodes[index_destination].name);
    else if(strcmp(nodes[index_origin].name, (char*)"")!=0 && strcmp(nodes[index_destination].name, (char*)"")==0)
    printf("\nDo you want to perform A* algorithm from %s to node with id %lu? (Y/N)", nodes[index_origin].name, nodes[index_destination].id);
    else printf("\nDo you want to perform A* algorithm from node with id %lu to node with id %lu? (Y/N)", nodes[index_origin].id, nodes[index_destination].id);


    return 0;
}