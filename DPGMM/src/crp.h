#ifndef _CRP_H
#define _CRP_H

#include "list/list.h" /* List and ListNode structs */

/* 
 * Returns the ijth element of the upper packed triangular matrix A
 * Only handles the case when i <= j, since that is all we need
*/
#define TRIU(U,i,j) U[i+(j+1)*j/2]

/* Table struct */
typedef struct Table {
	unsigned int size; // Number of customers at the table
	/* Normal-inverse-Wishart hyperparameters */
	double *xi; // Location
	double kappa; // Precision scale factor
	double nu;	// Degrees of freedom
	double *psi; // Right Cholesky factor of inverse scale matrix (uses packed upper triangular storage)
	double logdetpsi; // Log determinant of the Cholesky factor
} Table;

/* Chinese restaurant object */
typedef struct ChineseRestaurant {
	double alpha; /* Concentration parameter */
	Table table_prior; /* Table struct containing prior hyperparameters */
	ListNode **assigned_tables; /* Maps customers to their table. Stores list node ptrs rather than table ptrs to allow list node removal without search. */
	List *occupied_tables; /* Linked list of non-empty tables */
	List *empty_tables; /* Linked list of empty tables */
} ChineseRestaurant;

/* Iterates over both the elements and their value in a list using an index. */
#define LIST_ENUMERATE(LIST, INDEX, NODE, VALUE)\
    ListNode *NODE = NULL;\
	Table *VALUE = NULL;\
	int INDEX;\
    for(INDEX = 0, NODE=LIST->first, VALUE = Node_value(NODE);\
    INDEX < LIST->count;\
    INDEX++, NODE = NODE->next, VALUE = Node_value(NODE))

/* Initialises Chinese restaurant processs */
ChineseRestaurant *crp_init(const char* filename, const double alpha, const double xi[], const double kappa, const double nu, const double psi[]);

/* Adds a new empty table and assigns it the prior hyperparameters*/
void add_new_table(ChineseRestaurant *cr);

/* Resets the table parameters to the prior hyperparameters */
void clear_empty_table(ChineseRestaurant *cr, ListNode *tablenode);

/* Returns a pointer to the next empty table in the restaurant */
Table *next_available_table(ChineseRestaurant *cr);

/* Seats customer at the given table and updates the table hyperparameters accordingly */
void stand_customer(ChineseRestaurant *cr, const int customer_index);

/* Draws a table using CRP */
ListNode *crp_draw(ChineseRestaurant *cr, const int customer_index);

/* Removes customer from their current table and updates the table hyperparameters accordingly */
void seat_customer(ChineseRestaurant *cr, const int customer_index);

/* Updates the table hyperparameters after a customer is added/removed from table */
void table_update(Table *table, const int customer_index, const int sign);

/* Computes the (unnormalised) conditional log likelihood of a customer joining the table */
double log_likelihood(Table *table, const double *customer);

/* Updates the table assignments in the restaurant */
void update_table_assignments(ChineseRestaurant *cr);

/* Updates the concentration parameter */
void update_alpha(ChineseRestaurant *cr);

/* Updates/downdates the upper Cholesky factor U after a rank-1 modification. */
void choldate(double *U, const double *x, const int sign);

/* Computes the logarithm of the determinant for an upper packed triangular matrix U. */
double logdet(const double *U);

/* Evenly spaced numbers over a specified interval. */
void linspace(const double start, const double stop, const int num, double *output);

/* Evaluates the posterior predictve distribution on a meshgrid. Used to make contour plot. */
void posterior_predictive_surface(ChineseRestaurant *cr, const int nx, const double *xgrid, const int ny, const double *ygrid);

#endif