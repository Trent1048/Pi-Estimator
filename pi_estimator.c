/*  Cpt S 411, Introduction to Parallel Computing
 *  Trent Bultsma, Programming project 4
 */

// gcc pi_estimator.c -o pi -fopenmp -lm

#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]) {

    long long int dart_throws;
	if(argc<2) {
		printf("Usage: pi <n: number of darts to throw> <p: number of threads>\n");
		exit(1);
	}
	dart_throws = atoll(argv[1]);

	int thread_count=1;
	if(argc==3) {
		thread_count = atoi(argv[2]);
		assert(thread_count >= 1);
    }
	omp_set_num_threads(thread_count);

    // We throw dart_throws number of darts randomly within a 1 by 1 
    // square with a circle of diameter 1 right in the center. The 
    // ratio of darts in the circle (hits) to darts thrown is used
    // to roughly estimate pi since the (area of the circle) / (area 
    // of the square) is equal to pi / 4.

    srand48(0);
    double circle_center = 0.5;
    double squared_circle_radius = 0.25;

    double time = omp_get_wtime();

    long long int hits = 0;
    #pragma omp parallel for reduction(+:hits)
    for(long long int i = 0; i < dart_throws; i++) {

        int seed = (omp_get_thread_num() + 1) * i;

        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        double squared_dist_from_center = pow(x - circle_center, 2) + pow(y - circle_center, 2);
        if (squared_dist_from_center < squared_circle_radius) {
            hits++;
        }
    }
    
    time = omp_get_wtime() - time;

    double pi_actual_value = 3.1415926535897932384; // from google
    double pi_estimated_value = (double)hits / dart_throws * 4.0;

    int accuracy = 0;
    long long int decimal_place_to_check = 1;
    while (1) {
        if ((long long int)(pi_actual_value * decimal_place_to_check) ==
            (long long int)(pi_estimated_value * decimal_place_to_check)) {
            accuracy++;
            decimal_place_to_check *= 10;

            // the actual value for pi in this program doesn't even have that many digits
            if (accuracy > 100) break;
        }
        else {
            break;
        }
    }

    printf("Estimate for pi = %.17g\nAccuracy = %d decimal places\nElapsed time = %f seconds\n",
        pi_estimated_value, accuracy, time);
}
