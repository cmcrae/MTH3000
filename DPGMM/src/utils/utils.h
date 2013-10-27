#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>

typedef enum {
	CHAR, INT, FLOAT, DOUBLE
} TYPE;

/* Import data from file to 2D-array */
void import_data(const char *filename, const int rows, const int cols, double **data);

/* Export data from array to file */
void export_data(TYPE dtype, const void *data, const int length, const char *filename);
#endif