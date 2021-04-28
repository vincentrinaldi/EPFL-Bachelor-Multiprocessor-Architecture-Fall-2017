/*
============================================================================
Filename    : algorithm.c
Author      : Vincent Rinaldi
SCIPER		: 239759
============================================================================
*/

#define input(i,j) input[(i)*length+(j)]
#define output(i,j) output[(i)*length+(j)]

void simulate(double *input, double *output, int threads, int length, int iterations)
{
    /* Your code goes here */
    
    omp_set_num_threads(threads);
    
    int i;
    int j;
    int k = iterations;
    
	#pragma omp parallel shared ( input, output, k ) private ( i, j )
	{
		while (k > 0) {
			
			/* Ensure that every threads wait each other after evaluating the condition. */
			#pragma omp barrier
			
			/* Update the number of remaining iterations. */
			#pragma omp single nowait
			{
				k--;
			}
			
			/* Save the old solution. */
			#pragma omp for schedule(static)
			for ( i = 1; i < length - 1; i++ ) {
				for ( j = 1; j < length - 1; j++ ) {
					input(i,j) = output(i,j);
				}
			}
		
			/* Determine the new estimate of the solution at the interior points. */
			#pragma omp for nowait schedule(static)
			for ( i = 1; i < length - 1; i++ ) {
				for ( j = 1; j < length - 1; j++ ) {
					if (!((i == length/2 - 1 && j == length/2 - 1) || (i == length/2 && j == length/2 - 1) || (i == length/2 - 1 && j == length/2) || (i == length/2 && j == length/2))) {
						output(i,j) = ( input(i-1,j-1) + input(i-1,j) + input(i-1,j+1) + input(i,j-1) + input(i,j) + input(i,j+1) + input(i+1,j-1) + input(i+1,j) + input(i+1,j+1) ) / 9.0;
					}
				}
			}
			
		}
	}
	
}
