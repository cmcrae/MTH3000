#include "src/crp.h" // crp_init, update_table_assignments, update_alpha, LIST_ENUMERATE
#include "src/random/random.h" // initialise_rngs
#include "src/utils/utils.h" // export_data
#include <stdio.h> // printf

#define BURNIN  0 // Burn-in
#define SAMPLES 10000 // Number of posterior samples to draw

/************************** Global variables **********************************/
const int N = 272; // Number of data points
const int D = 2; // Dimensionality of data.
extern double **customers; // NxD array of data. Defined in crp_init() of crp.c.
/******************************************************************************/

int main(int argc, char *argv[])
{
	// Seeds rng with system time
	initialise_rngs();

	// Component hyperparameters
	double xi[] = {0.,0.};
	double psi[] = {1., 0.,1.};
	double kappa = 0.0001;
	double nu = 2.;

	// Concentration parameter
	double alpha = 1.0;

	// Initialise CRP
	ChineseRestaurant *cr = crp_init(argv[1], alpha, xi, kappa, nu, psi);

	// Perform burn-in
	printf("Performing burn-in.\n");
	for(int i=0; i<BURNIN; i++) {
		update_table_assignments(cr);
		update_alpha(cr);
	}
	
	// Sample posterior
	printf("Burn-in complete. Sampling will now begin.\n");
	for(int i=0; i<SAMPLES; i++) {
		update_table_assignments(cr);
		update_alpha(cr);
		//Export samples to file
		export_data(INT, &(cr->occupied_tables->count), 1, "output/occupied_tables.txt");
		export_data(DOUBLE, &(cr->alpha), 1,"output/alpha.txt");

		LIST_ENUMERATE(cr->occupied_tables, j, node_j, table_j){
			export_data(DOUBLE, table_j->xi, D, "output/xi.txt");
			export_data(DOUBLE, table_j->psi, D*(D+1)/2, "output/psi.txt");
			export_data(DOUBLE, &(table_j->nu), 1, "output/nu.txt");
			export_data(DOUBLE, &(table_j->kappa), 1, "output/kappa.txt");
			export_data(INT, &(table_j->size), 1, "output/table_sizes.txt");
		}
		
	}
	printf("Sampling complete.\n");
	
	// Create contour plot of predictive posterior distribution
	printf("Evaluating posterior predictive distribution on meshgrid.\n");
	// Create a mesh grid
	int nx = 500;
	int ny = 500;
	double xgrid[nx];	
	double ygrid[ny];
	linspace(0,7, nx, xgrid);
	linspace(40, 100, ny, ygrid);
	// Evaluate predictive posterior on mesh grid
	posterior_predictive_surface(cr, nx, xgrid, ny, ygrid);

	printf("Complete.\n");
	return 0;
}