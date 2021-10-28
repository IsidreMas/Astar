# include "functions.h"

typedef struct DijkstraState{
  double *g;
  unsigned *parent;
  bool *expanded;
} DijkstraState;

DijkstraState Dijkstra(node *nodes, unsigned nnodes, unsigned index_origin, unsigned index_destination)
{
    DijkstraState s;
    double dist_aux;
    unsigned i, node_min;

    if((s.expanded = (bool *) malloc(nnodes*sizeof(bool)) ) == NULL)
        ExitError("when allocating memory for expanded vector", 15);
    if((s.g = (double *) malloc(nnodes*sizeof(double)) ) == NULL)
        ExitError("when allocating memory for dist vector", 15);
    if((s.parent = (unsigned *) malloc(nnodes*sizeof(unsigned)) ) == NULL)
        ExitError("when allocating memory for parent vector", 15);

    for(i=0u; i<nnodes; i++)
    {
        s.expanded[i]=false;
        s.g[i]=INFINITY;
    }
    s.parent[index_origin]=nnodes;
    s.g[index_origin]=0.0;
    
    Heap *Pq = CreateHeap(nnodes);
    insert(Pq, s.g[index_origin], index_origin);

    while(Pq->count!=0)
    {
        node_min = PopMin(Pq);
        s.expanded[node_min]=true;

        if(node_min == index_destination)break;

        for(i=0; i<nodes[node_min].nsucc; i++)
        {
            if(!s.expanded[nodes[node_min].successors[i]])
            {
                dist_aux = s.g[node_min] 
                + get_distance(nodes[node_min].lat, nodes[node_min].lon, nodes[nodes[node_min].successors[i]].lat, nodes[nodes[node_min].successors[i]].lon);
                if(s.g[nodes[node_min].successors[i]]>dist_aux)
                {
                    if(s.g[nodes[node_min].successors[i]]==INFINITY)insert(Pq, dist_aux, nodes[node_min].successors[i]);
                    else decreasePriority(Pq, dist_aux, nodes[node_min].successors[i]);
                    s.g[nodes[node_min].successors[i]] = dist_aux;
                    s.parent[nodes[node_min].successors[i]] = node_min;
                }
            }
        }
    }
    
    return s;
}

int main (int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("\nUsage: ./Astar 'GRAPH_BINARY_FILE' 'ORIGIN_ID' 'DESTINATION_ID'\n");
        ExitError("The arguments expected were not given", 32);
    }

    FILE *binary_file;
    unsigned long ntotnsucc, ntotnamechar;
    clock_t local_time, global_time;
    unsigned *allsuccessors, i, nnodes;
    char *allnames;
    double local_CPU_time, global_CPU_time;
    node *nodes;

    global_time=clock();
    local_time=clock();

    if ((binary_file = fopen (argv[1], "rb")) == NULL)
        ExitError("the data file does not exist or cannot be opened", 11);

    printf("The binary file %s was opened succesfully.\n\n", argv[1]);
    printf("Reading binary file...\n\n");
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

    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("The binary file was read in %f CPU seconds.\n\n", local_CPU_time);
    printf("Searching indices of origin and destination...\n");
    local_time = clock();

    unsigned long id_origin = strtoul(argv[2],(char **)NULL, 10), id_destination = strtoul(argv[3],(char **)NULL, 10);
    unsigned index_origin, index_destination, path_node;

    if(!binarysearch(id_origin, nodes, nnodes, &index_origin))
        ExitError("The origin ID is not in the graph", 11);
    if(!binarysearch(id_destination, nodes, nnodes, &index_destination))
        ExitError("The origin ID is not in the graph", 11);
    
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("The indices were found in %f CPU seconds.\n\n", local_CPU_time);

    printf("Performing Dijkstra algorithm...\n");
    local_time = clock();

    DijkstraState result = Dijkstra(nodes, nnodes, index_origin, index_destination);

    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("Astar algorithm found an optimal path in %f CPU seconds.\n\n", local_CPU_time);
    printf("Saving results in output file...\n");
    local_time = clock();

    path_node = index_destination;

    FILE *channeltofile;
    
    channeltofile = fopen("Dijkstra_output.txt","w"); // "w" means "write"
    if(channeltofile == NULL){
        printf("We cannot create the file!");
        ExitError("The file could not be opened", 15);
    }
    while (result.parent[path_node]!=nnodes)
    {
        fprintf(channeltofile, "%f,%f\n", nodes[path_node].lon, nodes[path_node].lat);
        path_node = result.parent[path_node];
    }
    fclose(channeltofile); // This file must exist now!

    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("The file output.txt was written in %f CPU seconds.\n\n", local_CPU_time);
    global_CPU_time = (double)(clock()-global_time)/CLOCKS_PER_SEC;
    printf("The total computation time was %f CPU seconds.\n\n", global_CPU_time);

    printf("The shortest distance found is %f m\n", result.g[index_destination]);

    return 0;
}