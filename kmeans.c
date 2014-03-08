#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

/*
 * Store one data point's (or one cluster centroid's) 
 * x and y co-ordinates.
 */
typedef struct datapoint {
	float x, y;
} datapoint;

/*
 * Reads 2-D floating point csv data from file,
 * storing it in an array of datapoints.
 * The array is returned and lines is set to the number of data points.
 */
datapoint *readData(FILE *file, int *lines)
{	
	int ch, i, rv; 
	datapoint *dp;
	*lines = 0;

	while ((ch = fgetc(file)) != EOF) 
	{
		if (ch == '\n')
		{
			(*lines)++;
		}
	}

	dp = (datapoint*) malloc(*lines * sizeof(datapoint));
	if (dp == NULL) 
	{
		perror("Error allocating memory for data");
		return NULL;
	}
	
	rewind(file);
	
	for (i = 0; i < *lines; i++)
	{
		rv = fscanf(file, "%f, %f\n", &dp[i].x, &dp[i].y);
		if(rv != 2)
		{
			printf("Error reading input data from file");
			free(dp);
			return NULL;
		}
	}
	
	return dp;
}

/*
 * Read cluster names, one per line, from file. 
 */
char** readNames(FILE *nameFile, int K)
{
	int k, l;
	char **names;
	
	names = (char**) malloc(K * sizeof(char*));
	
	for (k = 0; k<K; k++)
	{
		names[k] = (char*) malloc(32);
		if (names == NULL)
		{
			perror("Error allocating memory for names");
			for (l = 0; l<k-1; l++)
			{
				free(names[l]);
			}
			free(names);
			return NULL;
		}
		
		if (fscanf(nameFile, "%31s\n", names[k]) == EOF)
		{
			printf("Not enough names in name file\n");
			for (l = 0; l<k; l++)
			{
				free(names[l]);
			}
			free(names);
			return NULL;
		}
	}
	
	return names;
}

/*
 * Perform one iteration of K-means clustering
 */
float kmeans(int N, int K, datapoint *dp, datapoint *cc, float *d, 
			 int *r)
{
	int k, n, numPoints[K];
	float shortestDist, error;
	double xtot[K], ytot[K];
	
	for (k = 0; k<K; k++)
	{
		xtot[k] = ytot[k] = 0.0;
		numPoints[k] = 0.0;
	}
	
	// Expectation -- assign points to clusters
	for (n = 0; n<N; n++)
	{
		// Find squared Euclidean distance from each data point 
		// to each centre
		for (k = 0; k<K; k++)
		{
			
			d[k] = pow((dp[n].x - cc[k].x), 2) + 
			       pow((dp[n].y - cc[k].y), 2);
		}
		
		// Assign each data point to its nearest cluster
		r[n] = 0;
		shortestDist = d[0];
		for (k = 1; k<K; k++)
		{
			if (d[k] < shortestDist)
			{
				shortestDist = d[k];
				r[n] = k;
			}
		}
		
		// Accumulate data point x and y co-ordinates for each cluster		
		numPoints[r[n]]++;
		xtot[r[n]] += dp[n].x;
		ytot[r[n]] += dp[n].y;
	}
	
	// Maximisation -- update cluster centres
	for (k = 0; k<K; k++)
	{
		// Assign means of accumulated data points 
		// to cluster centroids.
		if (numPoints[k] != 0)
		{
			cc[k].x = (float)(xtot[k] / (double)numPoints[k]);
			cc[k].y = (float)(ytot[k] / (double)numPoints[k]);
		}
	}
	
	// Find error according to this iteration's calculation of cluster 
	// centres.  Compare with the last iteration's to check for 
	// convergence.
	error = 0.0;
	
	for (n = 0; n<N; n++)
	{
		k = r[n];
		error += pow((dp[n].x - cc[k].x), 2) + 
		         pow((dp[n].y - cc[k].y), 2);
	}
	
	return error;
}

/* 
 * Append cluster centroids to cFile and data point assignments 
 * to aFile.
 */
void saveData(FILE *cFile, FILE *aFile, int N, int K, int *r, 
			  datapoint *cc)
{
	int n, k;
	
	// Write out data point assignments
	for (n=0; n<N; n++)
	{
		fprintf(aFile, "%i\n", r[n]);
	}
	
	// Write out cluster centroids
	for (k=0; k<K; k++)
	{
		fprintf(cFile, "%f, %f\n", cc[k].x, cc[k].y);
	}
}

/*
 * Write final error and assignments to file.
 */
void writeOutput(FILE *oFile, int *r, float error, char** names, int N)
{
	int n;
	
	fprintf(oFile, "error = %.3f\n", error);
	for (n=0; n<N; n++)
	{
		fprintf(oFile, "%s\n", names[r[n]]);
	}
}

int main()
{
	FILE *dataFile, *namesFile, *clusterFile;
	FILE *centroidsOutFile, *assignmentsOutFile, *outputFile;
	int N, K, iter, *r, k;
	datapoint *data, *cc;
	float *d, eOld, e;
	char **names;
	
	dataFile = fopen("input.csv", "r");
	if (dataFile == NULL)
	{
		perror("Error opening input file");
		return errno;
	}
	
	clusterFile = fopen("initialCentroids.csv", "r");
	if (clusterFile == NULL)
	{
		perror("Error opening initial centroids file");
		fclose(dataFile);
		return errno;
	}
	
	namesFile = fopen("names.txt", "r");
	if (namesFile == NULL)
	{
		perror("Error opening names file");
		fclose(dataFile);
		fclose(clusterFile);
		return errno;
	}

	data = readData(dataFile, &N);
	fclose(dataFile);
	if (data == NULL)
	{
		fclose(clusterFile);
		fclose(namesFile);
		return errno;
	}
	
	cc = readData(clusterFile, &K);
	fclose(clusterFile);
	if (cc == NULL)
	{
		fclose(namesFile);
		free(data);
		return errno;
	}
	
	names = readNames(namesFile, K);
	fclose(namesFile);
	if (names == NULL)
	{
		free(data);
		free(cc);
		return errno;
	}
	
	centroidsOutFile = fopen("outputCentroids.csv", "w");
	if (centroidsOutFile == NULL)
	{
		perror("Error opening centroids output file");
		free(data);
		free(cc);
		for (k = 0; k<K; k++)
		{
			free(names[k]);
		}
		return errno;
	}
	
	assignmentsOutFile = fopen("outputAssignments.csv", "w");
	if (assignmentsOutFile == NULL)
	{
		perror("Error opening assignments output file");
		fclose(centroidsOutFile);
		free(data);
		free(cc);
		for (k = 0; k<K; k++)
		{
			free(names[k]);
		}
		free(names);
		return errno;
	}
	
	outputFile = fopen("OUTPUT.TXT", "w");
	if (outputFile == NULL)
	{
		perror("Error opening output file");
		fclose(centroidsOutFile);
		fclose(assignmentsOutFile);
		free(data);
		free(cc);
		for (k = 0; k<K; k++)
		{
			free(names[k]);
		}
		free(names);
		return errno;
	}
	
	// Distance of datapoint from cluster
	d = (float*) malloc(K * sizeof(float)); 
	if (d == NULL)
	{
		perror("Error allocating memory for distances");
		free(data);
		free(cc);
		for (k = 0; k<K; k++)
		{
			free(names[k]);
		}
		free(names);
		return errno;
	}
	
	// Assignment of datapoint to cluster
	r = (int*) malloc(N * sizeof(int)); 
	if (r == NULL)
	{
		perror("Error allocating memory for assignments");
		free(data);
		free(cc);
		free(d);
		for (k = 0; k<K; k++)
		{
			free(names[k]);
		}
		free(names);
		return errno;
	}
	
	iter = 0;
	e = 0.0;
	
	do
	{
		eOld = e;
		e = kmeans(N, K, data, cc, d, r);
		printf("Iteration %i, error is %f\n", iter, e);
		saveData(centroidsOutFile, assignmentsOutFile, N, K, r, cc);
		iter++;
	} while(e != eOld);
	
	writeOutput(outputFile, r, e, names, N);
	
	fclose(centroidsOutFile);
	fclose(assignmentsOutFile);
	fclose(outputFile);
	
	free(d); 
	free(r);
	free(data);
	free(cc);
	
	for(k = 0; k<K; k++)
	{
		free(names[k]);
	}
	free(names);
	
	return 0;
}


