#!/usr/bin/bash

helpFunction()
{
   echo ""
   echo -e "\t-f CSV file of the OSM data to be used."
   echo -e "\t-o Origin ID"
   echo -e "\t-d Destination ID"
   echo -e "\t-a Algorithm to use (Dijkstra or Astar) Default: Astar"
   echo -e "\t-c Color of the path"
   exit 1 # Exit script after printing help
}

while getopts "f:o:d:a:c:" opt
do
   case "$opt" in
      f ) map_name="$OPTARG" ;;
      o ) origin_id="$OPTARG" ;;
      d ) destination_id="$OPTARG" ;;
      a ) algorithm="$OPTARG" ;;
      c ) color="$OPTARG" ;;
      ? ) helpFunction ;; # Print helpFunction in case parameter is non-existent
   esac
done

# Print helpFunction in case parameters are empty
map_name=${map_name-spain.csv}
# Begin script in case all parameters are correct

if [ ! -f ./source/Dijkstra.exe ]
then
    gcc -Wall -o ./source/Dijkstra.exe ./source/Dijkstra.c -Ofast
fi

if [ ! -f ./source/Astar.exe ]
then
    gcc -Wall -o ./source/Astar.exe ./source/Astar.c -Ofast
fi

if [ ! -f ./source/graph_builder.exe ]
then
    gcc -Wall -o ./source/graph_builder.exe ./source/graph_builder.c -Ofast
fi

if [ ! -f ./binaries/${map_name%.*}_graph.bin ]
then
    ./source/graph_builder.exe ./maps_data/$map_name ./binaries/${map_name%.*}_graph
fi

if [ ${algorithm-Astar} == "Dijkstra" ]
then
    ./source/Dijkstra.exe ./binaries/${map_name%.*}_graph.bin ${origin_id-240949599} ${destination_id-195977239}
else
    ./source/Astar.exe ./binaries/${map_name%.*}_graph.bin ${origin_id-240949599} ${destination_id-195977239}
fi

source "temporary_id.txt"

"D:\Program Files (x86)\R-4.0.2\bin\Rscript.exe" ./source/plotter.r ${algorithm-Astar} ${id_origin} ${id_destination} ${color-red}
start ./results/${algorithm-Astar}_${id_origin}_${id_destination}.html

rm temporary_id.txt
 