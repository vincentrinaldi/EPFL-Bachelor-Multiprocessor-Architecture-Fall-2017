/*
============================================================================
Filename    : pi.c
Author      : Vincent Rinaldi
SCIPER		 : 239759
============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "utility.h"

double calculate_pi (int num_threads, int samples);

int main (int argc, const char *argv[]) {

    int num_threads, num_samples;
    double pi;

    if (argc != 3) {
		printf("Invalid input! Usage: ./pi <num_threads> <num_samples> \n");
		return 1;
	} else {
        num_threads = atoi(argv[1]);
        num_samples = atoi(argv[2]);
	}

    set_clock();
    pi = calculate_pi (num_threads, num_samples);

    printf("- Using %d threads: pi = %.15g computed in %.4gs.\n", num_threads, pi, elapsed_time());

    return 0;
}


double calculate_pi (int num_threads, int samples) {
    double pi;

    /* Your code goes here */
    
    int i, count;
	double x, y;
	rand_gen gen;
	
	omp_set_num_threads(num_threads);

    #pragma omp parallel private(i, x, y, gen) shared(samples)
	{
		gen = init_rand();
		
		#pragma omp for reduction(+:count)
		for (i=0; i<samples; i++) {
			x = next_rand(gen);
			y = next_rand(gen);
			if (x*x + y*y <= 1.0) count++;
		}
	}
	
	double ratio = (double) count/samples;
	pi = 4*ratio;
	
    return pi;
}
