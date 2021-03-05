library(networkD3)
library(htmltools)
library(htmlwidgets)

args <- commandArgs(trailingOnly = TRUE)

nodes <- read.csv(file = args[2], sep=",")
edges <- read.csv(file = args[3], sep=",")
dt <- args[4]
output <- paste(args[5], ".html", sep="")

network <- forceNetwork(Links = edges, Nodes = nodes,
                    Source = 'source', Target = 'target', Value = 'value', linkWidth = JS("function(d) { return Math.pow(d.value, 1/5); }"),
                    NodeID = 'name', Nodesize = 'size', radiusCalculation = JS("Math.pow(d.nodesize, 1/5)"),
                    Group = 'group', legend = T, zoom = F, arrow = T, charge = -1,
                    opacity = 1, bounded = T, colourScale = JS('d3.scaleOrdinal()
          .domain(["blue", "red"])
         .range(["#000080", "#FF0000"]);'))

network <- htmlwidgets::prependContent(network, htmltools::tags$h1(paste0("Dataset: ", dt)))


saveNetwork(network, file = paste0(output), selfcontained = TRUE)
print(paste("Check ", output))
