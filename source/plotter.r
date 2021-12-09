#!/usr/bin/env Rscript

args = commandArgs(trailingOnly=TRUE)

install.packages('leaflet')
library(leaflet)
library(htmlwidgets)

(m<-leaflet(data = read.csv(paste("./results/",args[1],"_",args[2],"_",args[3],".csv", sep="", collapse=NULL)))%>%addTiles()%>%addPolylines(lat = ~lat, lng = ~lon, color = args[4]))
#(m<-leaflet(data = read.csv(paste("./results/","Astar","_","240949599","_","195977239",".csv", sep="", collapse=NULL)))%>%addTiles()%>%addPolylines(lat = ~lat, lng = ~lon, color ="red"))

saveWidget(m, file=paste("./plots/",args[1],"_",args[2],"_",args[3],".html", sep="", collapse=NULL))

