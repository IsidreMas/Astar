#!/usr/bin/env Rscript
args = commandArgs(trailingOnly=TRUE)

library(leaflet)
library(htmlwidgets)

(m<-leaflet(data = read.csv(paste("./results/",args[1],"_",args[2],"_",args[3],".csv", sep="", collapse=NULL)))%>%addTiles()%>%addPolylines(lat = ~lat, lng = ~lon, color = args[4]))
saveWidget(m, file=paste("./plots/",args[1],"_",args[2],"_",args[3],".html", sep="", collapse=NULL))
