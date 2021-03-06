# Astar (A*) algorithm
Astar assignment - A routing problem

Authors: Isidre Mas Magre & Martha Domhoefer

The problem consists in computing an optimal path (according to distance) from Basílica de Santa Maria del Mar (Plaça de Santa Maria) in Barcelona to the Giralda (Calle Mateos Gago) in Sevilla by using AStar algorithm over a graph constructed with data from OpenStreetMaps. The program developed to solve this problem was written mainly in C, but it also contains an R script to plot the resulting route in an interactive map using the Leaflet library and finally all the scripts are wrapped together with a bash script which compiles and executes the program in a user friendly interface. Additionally the program can find the optimal route between any two valid nodes of the map, using Astar algorithm as well as Dijkstra for comparison pursposes. It uses just a few command line arguments or none at all, since it has default values to solve the requested problem.

This docucument will only explain the steps to operate the program. For a detailed explanation of the methods used to develop the program a report has also been compiled as part of this assignment.

# Operation

We ran the complete code on two different machines, namely a HP Laptop with a windows 10 64-bit operating system, a 16GB RAM and a  Intel(R) Core(TM) i7-8750H CPU with a clock speed of 2.21 GHz. The other machine was a MacBook Air M1 using Mac OS with a clock speed of 3.2 GHz and 8GB of RAM. 

Before executing the program for the first time the datasets with the format specified in the description of the problem must be placed in the folder "./maps_data", by default the program will open the dataset "./maps_data/spain.csv". The next and final step to get the default results is execute the bash script named Routing.sh with the following command:

`bash Routing.sh`

If everything went well the program will build the graph, perform Astar algorithm and disiplay the solution on an interactive map. On subsequent executions it will be much faster since the graph will be already built. Additionally the following parameters can be tweked to control the program execution:

- -f: Name of the map to be used. By default is spain.csv
- -o: Origin node. The argument can be a valid node ID or coordinates in the format "LAT,LON". By default is set to the node ID 240949599.
- -d: Destination node. The argument can be a valid node ID or coordinates in the format "LAT,LON". By default is set to the node ID 195977239.
- -a: Algorithm to use, either Dijkstra or Astar. By default is set to Astar.
- -c: Color to draw the solution route in the leaflet plot. By default is set to red.

An example execution with all the mentioned arguments would look like this:


`bash Routing.sh -f cataluna.csv -o 41.61733,0.62554 -d 240949599 -a Dijkstra -c blue`

Be careful when specifying latitude and longitude, as there should not be any space after the coma and a point should be used to mark decimals.

When an error concerning the Rscript in line 59 or the following line occurs one can comment these lines and uncomment the last lines of the bash file. This problem occurs due to the differences between Windows and Mac OS and might also occure using Linux. 

That's all.



