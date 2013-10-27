#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/* Import data from file to 1D-array */
void import_data(const char *filename, const int rows, const int cols, double **data)
{
	FILE *fp; // file pointer

	fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr,"Cannot open %s\n", filename);
		exit(1);
	}

	for(int i=0; i<rows; i++) {
		for(int j=0; j<cols; j++) {
			fscanf(fp, "%lf%*[, \t\n]", &data[i][j]);
		}
	}

	if(fclose(fp) != 0) {
		fprintf(stderr,"Error in closing file %s\n", filename);
	}
}



/* Export data from array to file */
void export_data(TYPE dtype, const void *data, const int length, const char *filename)
{
	FILE *fp; // file pointer

	fp = fopen(filename, "a");
	if(fp == NULL) {
		fprintf(stderr,"Cannot open %s\n", filename);
	}

	switch(dtype) {
	case DOUBLE:
		for(int i=0; i<length; i++) {
			fprintf(fp, "%lf\t", ((double *)data)[i]);
		}
		break;
	case INT:
		for(int i=0; i<length; i++) {
			fprintf(fp, "%d\t", ((int *)data)[i]);
		}
		break;
	case FLOAT:
		for(int i=0; i<length; i++) {
			fprintf(fp, "%f\t", ((float *)data)[i]);
		}
		break;
	case CHAR:
		fprintf(fp, "%s", (char *)data);
		break;
	default:
		fprintf(stderr,"export_data received unknown data type.");
		break;
	}
	
	fprintf(fp, "\n");

	if(fclose(fp) != 0) {
		fprintf(stderr, "Error in closing file %s\n", filename);
	}
}