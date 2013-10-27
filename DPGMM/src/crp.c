#include "crp.h"
#include <Accelerate/Accelerate.h> // cblas_dscal, cblas_daxpy, cblas_dtpsv
#include <math.h> // log, exp, lgamma, sqrt, M_PI
#include <stdlib.h> // calloc
#include <string.h> // memcpy
#include "random/random.h" // randu 
#include "slicesample/slicesample.h"  // slice_sample_alpha
#include "utils/utils.h" // import_data, export_data
#include <stdio.h>

/********************* Global variables *************************/
extern const int N;	// Number of data points. Defined in main
extern const int D; // Dimensionality of data. Defined in main
double **customers; // Data points stored as an N by D array.
/****************************************************************/


/* Initialises Chinese restaurant processs */
ChineseRestaurant *crp_init(const char *filename, const double alpha, const double xi[], const double kappa, const double nu, const double psi[])
{
	ChineseRestaurant *cr = calloc(1, sizeof(ChineseRestaurant));

	// Allocate space for data
	customers = calloc(N,sizeof(double *));
	for(int i=0; i<N; i++) {
		customers[i] = calloc(D,sizeof(double));
	}
	// Import data
	import_data(filename, N, D, customers);

	// Set concentration parameter
	cr->alpha = alpha;
	
	// Set prior hyperparameters
	Table *table_prior = &cr->table_prior;
	table_prior->xi = calloc(D,sizeof(double));
	memcpy(table_prior->xi, xi , D*sizeof(double));
	table_prior->kappa = kappa;
	table_prior->nu = nu;
	table_prior->psi = calloc(D*(D+1)/2, sizeof(double));
	memcpy(table_prior->psi, psi, (D*(D+1)/2)*sizeof(double));
	table_prior->logdetpsi = logdet(psi);
	
	// Initially, no customers are seated. We represent this using NULL pointers
	cr->assigned_tables = calloc(N,sizeof(ListNode));
	
	// Create the lists of occupied and empty tables
	cr->occupied_tables = List_create();
	cr->empty_tables = List_create();
	
	// Prepare an empty table
	add_new_table(cr);
	
	return cr;
}



/* Adds a new empty table and assigns it the prior hyperparameters */
void add_new_table(ChineseRestaurant *cr)
{
	Table *new_table = calloc(1, sizeof(Table));
	new_table->size = 0;
	new_table->xi = calloc(D,sizeof(double));
	memcpy(new_table->xi, cr->table_prior.xi , D*sizeof(double));
	new_table->kappa = cr->table_prior.kappa;
	new_table->nu = cr->table_prior.nu;
	new_table->psi = calloc(D*(D+1)/2, sizeof(double));
	memcpy(new_table->psi, cr->table_prior.psi, (D*(D+1)/2)*sizeof(double));
	new_table->logdetpsi = cr->table_prior.logdetpsi;
	
	// Add new table to list of empty tables
	List_push(cr->empty_tables, new_table);
}



/* Resets the table parameters to the prior hyperparameters */
void clear_empty_table(ChineseRestaurant *cr, ListNode *tablenode)
{
	// Remove the empty table from the list of occupied tables
	Table *empty_table = List_remove(cr->occupied_tables,tablenode);

	// Reset the table hyperparameters to the prior hyperparameters
	empty_table->size = 0;
	memcpy(empty_table->xi, cr->table_prior.xi , D*sizeof(double));
	empty_table->kappa = cr->table_prior.kappa;
	empty_table->nu = cr->table_prior.nu;
	memcpy(empty_table->psi, cr->table_prior.psi, (D*(D+1)/2)*sizeof(double));
	empty_table->logdetpsi = cr->table_prior.logdetpsi;

	// Add the table to the list of empty tables
	List_push(cr->empty_tables, empty_table);
}



/* Returns a pointer to the next empty table in the restaurant */
Table *next_available_table(ChineseRestaurant *cr)
{
	if(List_count(cr->empty_tables) == 0) {
		add_new_table(cr);
	}
	return List_last(cr->empty_tables);
}



/* Draws a table using CRP */
ListNode *crp_draw(ChineseRestaurant *cr, const int customer_index)
{
	int k = List_count(cr->occupied_tables); // Number of occupied tables 
	double logp[k+1]; // Unnormalised log probabilities for sitting at each of the tables
	double maxlogp; // Largest unnormalised log probability. Used to normalise the probabilities
	Table *empty_table = next_available_table(cr);
	
	// Probability of sitting at an empty table
	logp[k] = log(cr->alpha) + log_likelihood(empty_table, customers[customer_index]);
	maxlogp = logp[k];

	// Probabilities of sitting at the occupied tables
	LIST_ENUMERATE(cr->occupied_tables, i, node_i, table_i) {
		logp[i] = log(table_i->size) + log_likelihood(table_i, customers[customer_index]);
		if(logp[i] > maxlogp) {
			maxlogp = logp[i];
		}
	}

	// Compute normalising constant without numerical overflow/underflow
	double Z = 0;
	for(i=0; i<k+1; i++) {
		Z += exp(logp[i] - maxlogp);
	}
	double logZ = log(Z) + maxlogp;

	// Draw a new table by samppling from the multinomial distribution over 
	// (p0,...,pk) where pj is the probability of sitting at table j.
	double u = randu();
	double cum_prob_j = 0; // cumulative probability
	
	LIST_ENUMERATE(cr->occupied_tables, j, node_j, table_j) {
		cum_prob_j += exp(logp[j] - logZ);
		if(u < cum_prob_j) {
			return node_j; // Table j was drawn
		}
	}
	 // Empty table was drawn 
	return NULL;
}



/* Removes customer from their current table and updates the table hyperparameters accordingly */
void stand_customer(ChineseRestaurant *cr, const int customer_index)
{
	// If the customer is not already standing up, then stand the customer from their current table
	if(cr->assigned_tables[customer_index] != NULL) {
		Table *table = cr->assigned_tables[customer_index]->value;
		table->size -= 1;
		if(table->size == 0) { // If the table is now empty, then mark it as such.
			clear_empty_table(cr, cr->assigned_tables[customer_index]);
		} else { // Otherwise, update table parameters to reflect the loss of a customer
			table_update(table,customer_index, -1);
		}
	}
}



/* Draws a new table for the customer to sit at using the CRP and updates the table hyperparameters accordingly */
void seat_customer(ChineseRestaurant *cr, const int customer_index)
{
	Table *drawn_table;
	ListNode *drawn_table_node =  crp_draw(cr,customer_index);

	if(drawn_table_node != NULL) { // Seat customer at an existing table
		cr->assigned_tables[customer_index] = drawn_table_node;
		drawn_table = drawn_table_node->value;
		drawn_table->size +=1;
	} else { // Seat at an empty table
		drawn_table = List_pop(cr->empty_tables);
		drawn_table->size +=1;
		List_push(cr->occupied_tables, drawn_table);
		cr->assigned_tables[customer_index] = cr->occupied_tables->last;
	}
	// Update table parameters
	table_update(drawn_table, customer_index, 1);
}



/* Updates the table hyperparameters after a customer is add/removed from table */
void table_update(Table *table, const int customer_index, const int sign)
{	
	// Ψₒ =  Ψ + σ(κ/κₒ)(x-ξ)(x-ξ)'
	double z[D];
	memcpy(z,customers[customer_index],D*sizeof(double));				
	double p = table->kappa/(table->kappa + sign);
	cblas_daxpy(D,-1,table->xi,1,z,1);
	cblas_dscal(D, sqrt(p),z,1);
	choldate(table->psi, z, sign);
	table->logdetpsi = logdet(table->psi);

	// νₒ = ν + σ 
	table->nu += sign;

	// κₒ = κ + σ
	table->kappa += sign;

	// ξₒ = (κξ + σx)/κₒ
	cblas_dscal(D, p,table->xi,1);
	cblas_daxpy(D,sign/table->kappa,customers[customer_index],1,table->xi,1);
}



/* Updates the table assignments in the restaurant */
void update_table_assignments(ChineseRestaurant *cr)
{
	for(int i=0; i<N; i++) {
		// Stand the customer so we can reseat them
		stand_customer(cr, i);
		// Draw new table for the customer and seat them
		seat_customer(cr, i);		
	}
}



/* Updates the concentration parameter */
void update_alpha(ChineseRestaurant *cr)
{
	// Slice sampling with slice width of 1.0
	cr->alpha = slice_sample_alpha(cr->alpha, List_count(cr->occupied_tables), N, 1.0);
}



/* Computes the log posterior predictive probability of a customer joining the table */
double log_likelihood(Table *table, const double *customer)
{
	double result;
	double z[D]; // tmp storage for the result of L⁻¹(x-ξ)
	memcpy(z, customer, D*sizeof(double));
	cblas_daxpy(D,-1.0,table->xi,1,z,1);
	cblas_dtpsv(CblasColMajor,CblasUpper,CblasTrans,CblasNonUnit,D,table->psi,z,1);
	result = -0.5*(table->nu+1)*log(1 +  cblas_ddot(D,z,1,z,1)*table->kappa/(table->kappa + 1));
	result += lgamma( 0.5*(table->nu + 1) );
    result -= lgamma( 0.5*(table->nu + 1 - D) );
    result += 0.5*D * log( M_PI*(table->kappa/(table->kappa + 1)) );
    result -= table->logdetpsi;
    return result;
}



/* Updates/downdates the upper Cholesky factor U after a rank-1 modification. */
void choldate(double *U, const double *x, const int sign)
{
	double y[D];
    memcpy(y, x, D*sizeof(double));
    int i,j;
    double c,r,s; /* Temp variables */
    for(i=0; i<D; i++) {
        r = sqrt(TRIU(U,i,i)*TRIU(U,i,i) + sign*y[i]*y[i]);
        c = r/TRIU(U,i,i);
        s = y[i]/TRIU(U,i,i);
        TRIU(U,i,i) = r;
        for(j=i+1; j<D; j++) {
            TRIU(U,i,j) = (TRIU(U,i,j) + sign*s*y[j])/c;
            y[j] = c*y[j] - s*TRIU(U,i,j);
        }
    }
}




/* Computes the logarithm of the determinant for an upper packed triangular matrix U. */
/* This is simply the sum of the logarithms of the diagonal entries. */
double logdet(const double *U)
{
    double result = 0;
    int i;
    for(i=0; i<D; i++) {
        result += log(TRIU(U,i,i));
    }
    return result;
}

/* Evenly spaced numbers over a specified interval. */
void linspace(const double start, const double stop, const int num, double *output)
{
	output[0] = start;
	double step = (stop - start)/(num-1);
	for(int i=1; i<num; i++) {
		output[i] = output[i-1] + step;
	}
}

/* Evaluates the posterior predictve distribution on a meshgrid. Used to make a contour plot. */
/* Only useful for 2D target distribution */
void posterior_predictive_surface(ChineseRestaurant *cr, const int nx, const double *xgrid, const int ny, const double *ygrid)
{
	// We evaluate the pred. posterior distribution on one row of the mesh grid at a time, save the result, then repeat.
	double output[nx];
	double x[2]; // Current point in mesh grid
	Table *empty_table = next_available_table(cr);
	for(int j=0; j<ny; j++) {
		x[1] = ygrid[j];
		for(int i=0; i<nx; i++) {
			x[0] = xgrid[i];
			output[i] = (cr->alpha/(cr->alpha + cr->occupied_tables->count)) * exp(log_likelihood(empty_table, x));
			LIST_ENUMERATE(cr->occupied_tables, k, node_k, table_k) {
				output[i] += (table_k->size/(cr->alpha + cr->occupied_tables->count)) * exp(log_likelihood(table_k, x));
			}
		}
		export_data(DOUBLE, output, nx, "plots/post_pred_surf.txt");
	}
	export_data(DOUBLE, xgrid, nx, "plots/xgrid.txt");
	export_data(DOUBLE, ygrid, ny, "plots/ygrid.txt");
}