# include "functions.h"

int main(int argc, char *argv[]){
    FILE *nodesdata;
    node *nodes;
    clock_t local_time, global_time;
    size_t bufsize = 0;
    unsigned nnodes=0UL,i, field;
    char *buffer, *item;
    double local_CPU_time, global_CPU_time;
    unsigned short nsucc_max = 0U;

    if(argc != 3)
        ExitError("Expected arguments were not given.\n\nUsage: ./graph_builder 'PATH_TO_NODES_DATAFILE' 'OUTPUT_BINARY_FILE_NAME'", 32);

    global_time=clock();
    if((nodesdata=fopen(argv[1],"r"))==NULL)
        ExitError("Couldn't acces the nodes data file", 32);
    printf("\nThe file %s was opened succesfully.\n\n", argv[1]);

    // Counts the number of nodes    
    printf("Counting nodes...\n");
    local_time = clock();
    if((buffer=(char *)malloc(bufsize * sizeof(char)))==NULL)
        ExitError("Couldn't allocate the memory for buffer", 32);
    while(getline(&buffer, &bufsize, nodesdata))
    {
        item = strtoke(buffer,"|");
        if (strcmp(item, (char *)"node") == 0)nnodes++;
        else if(strcmp(item, (char *)"way") == 0)break;
    }
    rewind(nodesdata);
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("%u nodes were found in %f CPU seconds.\n\n", nnodes, local_CPU_time);

    //Allocates memory for nodes.
    printf("Reading node lines...\n");
    local_time = clock();
    if((nodes=(node *)malloc(nnodes*sizeof(node)))==NULL)
        ExitError("Couldn't allocate memory for nodes", 32);

    // Skips the first lines that are not nodes.
    do
    {
        getline(&buffer, &bufsize, nodesdata);
        item = strtoke(buffer,"|");
    }while(strcmp(item, (char *)"node") != 0);

    // Reads the data in the nodes lines and store it in the vector nodes.
    for(i=0UL;i<nnodes;i++)
    {
        field = 1U;
        while (item != NULL)
        {
            if(field == 2U)
            {
                nodes[i].id = strtoul(item,(char **)NULL, 10);
                if(i>0U && nodes[i-1].id>nodes[i].id)
                    ExitError("The nodes id's are not sorted in ascending order. That's necessary to perform binary search.", 32);
            }
            if(field == 3U)
            {   
                if((nodes[i].name = (char *) malloc(strlen(item)*sizeof(char))) == NULL)
                    ExitError("Couldn't allocate the memory for names", 32);
                strcpy(nodes[i].name, item);
            }
            if(field == 10U)nodes[i].lat = atof(item);
            if(field == 11U)nodes[i].lon = atof(item);
            field++;
            item = strtoke(NULL, "|");
        }
        nodes[i].nsucc=0U;
        if((nodes[i].successors=(unsigned *)malloc(nodes[i].nsucc*sizeof(unsigned)))==NULL)
            ExitError("Couldn't allocate the memory for successors", 32);

        getline(&buffer, &bufsize, nodesdata);
        item = strtoke(buffer,"|");
    }
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("Node lines were read in %f CPU seconds.\n\n", local_CPU_time);
    printf("Reading way lines and assigning node successors...\n");
    local_time = clock();
    // Reads the consecutive nodes of the way lines and keep two in memory: previousnodeindex and nodeindex.
    unsigned long nodeid, previousnodeid;
    unsigned nodeindex, previousnodeindex;
    bool first_node, repeated_node, oneway; //This are flags to deal with the inconsistencies in the file and reproduce the successors in twoway roads.

    while(strcmp(item, (char *)"way") == 0)
    {
        first_node = true;
        field = 1U;
        oneway = false;

        // Skips the first fields before the first existing node member of the way and sets the flag oneway if it's true.
        while(item!= NULL && first_node)
        {
            if(field > 9U)
            {
                previousnodeid = strtoul(item,(char **)NULL, 10);
                if(!binarysearch(previousnodeid, nodes, nnodes, &previousnodeindex))
                {
                    field++;
                    item = strtoke(NULL, "|");
                    continue;
                }
                first_node = false;
            }
            else if(field == 8U)if(strcmp(item, (char *)"oneway")==0)oneway = true;
            field++;
            item = strtoke(NULL, "|");
        }

        // Deals with the inconsistencies of the file and properly stores the successors of each node.
        while (item != NULL)
        {     
            nodeid = strtoul(item,(char **)NULL, 10);
            if(!binarysearch(nodeid, nodes, nnodes, &nodeindex))
            {
                item = strtoke(NULL, "|");
                continue;
            }
            repeated_node=false;
            for(i=0UL;i<nodes[previousnodeindex].nsucc;i++)
            {
                if(nodes[previousnodeindex].successors[i]==nodeindex)repeated_node = true;
            }
            if(!repeated_node)
            {
                if((nodes[previousnodeindex].successors=(unsigned *)realloc(nodes[previousnodeindex].successors, (nodes[previousnodeindex].nsucc + 1u)* sizeof(unsigned)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                nodes[previousnodeindex].successors[nodes[previousnodeindex].nsucc] = nodeindex;
                nodes[previousnodeindex].nsucc++;
                if(nodes[previousnodeindex].nsucc > nsucc_max)nsucc_max=nodes[previousnodeindex].nsucc;
            }
            if(!oneway)
            {
                repeated_node = false;
                for(i=0UL;i<nodes[nodeindex].nsucc;i++)
                {
                    if(nodes[nodeindex].successors[i]==previousnodeindex)repeated_node = true;
                }
                if(!repeated_node)
                {
                    if((nodes[nodeindex].successors=(unsigned *)realloc(nodes[nodeindex].successors, (nodes[nodeindex].nsucc + 1u)* sizeof(unsigned)))==NULL)
                    ExitError("Couldn't reallocate the memory for successors", 32);
                    nodes[nodeindex].successors[nodes[nodeindex].nsucc]=previousnodeindex;
                    nodes[nodeindex].nsucc++;
                    if(nodes[nodeindex].nsucc > nsucc_max)nsucc_max=nodes[nodeindex].nsucc;
                }
            }
            previousnodeindex=nodeindex;
            item = strtoke(NULL, "|");
        }
        getline(&buffer, &bufsize, nodesdata);
        item = strtoke(buffer,"|");
    }
    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    printf("Way lines were read and node successsors assigned in %f CPU seconds.\n\n", local_CPU_time);
    fclose(nodesdata);

    // Writes the binary file with the formatted graph, names and successors are writen in a single vector and need to be reasigned with pointers when reading.
    printf("Writing graph into the binary file...\n");
    local_time=clock();
    FILE *binary_file;

    // Computing the total number of successors and characters in names
    unsigned long ntotnsucc=0UL, ntotnamechar=0UL;
    for(i=0U; i < nnodes; i++)
    {
        ntotnsucc += nodes[i].nsucc;
        ntotnamechar += strlen(nodes[i].name)+1;
    }

    // Setting the name of the binary file
    char *binary_file_name = argv[2];
    strcat(binary_file_name, ".bin");
    if ((binary_file = fopen (binary_file_name, "wb")) == NULL)
        ExitError("the output binary data file cannot be opened", 31);
    // Global data −−− header 
    if( fwrite(&nnodes, sizeof(unsigned), 1, binary_file) + fwrite(&ntotnsucc, sizeof(unsigned long), 1, binary_file) + fwrite(&ntotnamechar, sizeof(unsigned long), 1, binary_file)!= 3 )
        ExitError("when initializing the output binary data file", 32);
    // Writing all nodes 
    if( fwrite(nodes, sizeof(node), nnodes, binary_file) != nnodes )
        ExitError("when writing nodes to the output binary data file", 32);
    // Writing sucessors in blocks 
    for(i=0UL; i < nnodes; i++)if(nodes[i].nsucc)if( fwrite(nodes[i].successors, sizeof(unsigned), nodes[i].nsucc, binary_file) != nodes[i].nsucc )
        ExitError("when writing edges to the output binary data file", 32);
    // Writing names in blocks
    for(i=0UL; i < nnodes; i++)if( fwrite(nodes[i].name, sizeof(char), strlen(nodes[i].name)+1, binary_file) != strlen(nodes[i].name)+1)
        ExitError("when writing edges to the output binary data file", 32);

    fclose(binary_file);

    local_CPU_time = (double)(clock()-local_time)/CLOCKS_PER_SEC;
    global_CPU_time = (double)(clock()-global_time)/CLOCKS_PER_SEC;
    printf("The graph was written into %s in %f CPU seconds.'\n\n", binary_file_name, local_CPU_time);

    valence_distribution(nsucc_max, nnodes, nodes);
    printf("The total computation time to build and write the graph was %f CPU seconds.\n\n", global_CPU_time);
    printf("Freeing allocated memory...\n");
    for(i=0UL;i<nnodes;i++)
    {
        free(nodes[i].name);
        free(nodes[i].successors);
    }
    free(nodes);
    free(buffer);

    printf("You can now execute the program './Astar %s'\n\n", argv[2]);

    return 0;
}


