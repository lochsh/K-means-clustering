K-means-clustering
==================

Performs K-means clustering on 2-D floating point data until convergence.  
Currently, cluster centroids must be specified externally in "initialCentroids.csv".  
Plan on adding function for Forgy initialisation in future.

The code requires three input files:  
-- "input.csv", which contains the input data  
-- "initialCentroids.csv" which contains the initial cluster centroids  
-- "names.txt", which contains the names of the clusters  


To run kmeans.c, ensure that these are present, then in a console:

$ make
$ ./kmeans


The MATLAB script "makegif.m" plots the cluster centroids and data
coloured according to the cluster it is assigned to.  These plots are
saved to .png files, which can be made into a .gif using ImageMagick by
entering into the console:

$convert -delay 50 *.png kmeans.gif

This is useful for visualising the progress of the algorithm, and also
for ensuring it is working as expected.
