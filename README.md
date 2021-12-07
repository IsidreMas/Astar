# Astar (A*) algorithm
Astar assignment - A routing problem

Authors: Isidre Mas Magre & Martha Domhoefer

The problem consists in computing an optimal path (according to distance) from Basílica de Santa Maria del Mar (Plaça de Santa Maria) in Barcelona to the Giralda (Calle Mateos Gago) in Sevilla by using AStar algorithm over a graph constructed with data from OpenStreetMaps. The program developed to solve this problem was written mainly in C, but it also contains an R script to plot the resulting route in an interactive map using the Leaflet library and finally all the scripts are wrapped together with a bash script which compiles and executes the program in a user friendly interface. Additionally the program can find the optimal route between any two valid nodes of the map, using Astar algorithm as well as Dijkstra for comparison pursposes. It uses just a few command line arguments or none at all, since it has default values to solve the requested problem.

This docucument will only explain the steps to operate the program. For a detailed explanation of the methods used to develop the program a report has also been compiled as part of this assignment.

# Operation

The program was tested in a MacBook Air M1 using Mac OS, compatibility with other systems was not checked and library dependencies (mainly for the Leaflet plotting) should be figured out by the user.

Before executing the program for the first time the datasets with the format specified in the description of the problem must be placed in the folder "./maps_data", by default the program will open the dataset "./maps_data/spain.csv". The next and final step to get the default results is execute the bash script named Routing.sh with the following command:

bash Routing.sh



