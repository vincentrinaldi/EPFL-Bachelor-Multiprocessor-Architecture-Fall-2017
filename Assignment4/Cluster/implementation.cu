/*
============================================================================
Filename    : algorithm.c
Author      : Vincent Rinaldi
SCIPER      : 239759
============================================================================
*/

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include <cuda_runtime.h>
using namespace std;


// CPU Baseline
void array_process(double *input, double *output, int length, int iterations)
{
    double *temp;

    for(int n=0; n<(int) iterations; n++)
    {
        for(int i=1; i<length-1; i++)
        {
            for(int j=1; j<length-1; j++)
            {
                output[(i)*(length)+(j)] = (input[(i-1)*(length)+(j-1)] +
                                            input[(i-1)*(length)+(j)]   +
                                            input[(i-1)*(length)+(j+1)] +
                                            input[(i)*(length)+(j-1)]   +
                                            input[(i)*(length)+(j)]     +
                                            input[(i)*(length)+(j+1)]   +
                                            input[(i+1)*(length)+(j-1)] +
                                            input[(i+1)*(length)+(j)]   +
                                            input[(i+1)*(length)+(j+1)] ) / 9;

            }
        }
        output[(length/2-1)*length+(length/2-1)] = 1000;
        output[(length/2)*length+(length/2-1)]   = 1000;
        output[(length/2-1)*length+(length/2)]   = 1000;
        output[(length/2)*length+(length/2)]     = 1000;

        temp = input;
        input = output;
        output = temp;
    }
}


// CUDA Kernel function
__global__ void kernel(double* input, double* output, int length) {

    int i   =	threadIdx.x + blockIdx.x * blockDim.x;
    int j   =	threadIdx.y + blockIdx.y * blockDim.y;

    int midSquare = (i == length/2 - 1 && j == length/2 - 1) || (i == length/2 && j == length/2 - 1) || (i == length/2 - 1 && j == length/2) || (i == length/2 && j == length/2);

    if ((i > 0) && (i < length-1) && (j > 0) && (j < length-1) && (!midSquare)) {
	output[(i)*(length)+(j)] = (input[(i-1)*(length)+(j-1)] +
				    input[(i-1)*(length)+(j)]   +
				    input[(i-1)*(length)+(j+1)] +
				    input[(i)*(length)+(j-1)]   +
				    input[(i)*(length)+(j)]     +
				    input[(i)*(length)+(j+1)]   +
				    input[(i+1)*(length)+(j-1)] +
				    input[(i+1)*(length)+(j)]   +
				    input[(i+1)*(length)+(j+1)] ) / 9;
    }
							
}


// GPU Optimized function
void GPU_array_process(double *input, double *output, int length, int iterations)
{
    //Cuda events for calculating elapsed time
    cudaEvent_t cpy_H2D_start, cpy_H2D_end, comp_start, comp_end, cpy_D2H_start, cpy_D2H_end;
    cudaEventCreate(&cpy_H2D_start);
    cudaEventCreate(&cpy_H2D_end);
    cudaEventCreate(&cpy_D2H_start);
    cudaEventCreate(&cpy_D2H_end);
    cudaEventCreate(&comp_start);
    cudaEventCreate(&comp_end);
    

    /* Preprocessing goes here */
    
    // declare device arrays
    double *input_d;
    double *output_d;
    
    // set device to be used for GPU executions
    cudaSetDevice(0);
    
    // set number of threads per block and number of blocks in the grid used in a kernel invocation 
    int numThreadsBlock = 8;
    int numBlocksGrid = (length % numThreadsBlock != 0) ? (length / numThreadsBlock + 1) : (length / numThreadsBlock);
    
    // makes coordinates of blocks and threads indexes to work in 2 dimensions
    dim3 numThreadsPerBlock(numThreadsBlock, numThreadsBlock);
    dim3 numBlocksInGrid(numBlocksGrid, numBlocksGrid);
    
    // allocate arrays on device 
    if (cudaMalloc((void **) &input_d, length*length*sizeof(double)) != cudaSuccess)
	cout << "error in cudaMalloc" << endl;
    if (cudaMalloc((void **) &output_d, length*length*sizeof(double)) != cudaSuccess)
	cout << "error in cudaMalloc" << endl;
		

    // copy from host to device step
    cudaEventRecord(cpy_H2D_start);
    /* Copying array from host to device goes here */   
    if (cudaMemcpy(input_d, input, length*length*sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
	cout << "error in cudaMemcpy" << endl;
    if (cudaMemcpy(output_d, output, length*length*sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
	cout << "error in cudaMemcpy" << endl;   
    cudaEventRecord(cpy_H2D_end);
    cudaEventSynchronize(cpy_H2D_end);
    
    
    // GPU calculation step
    cudaEventRecord(comp_start);
    /* GPU calculation goes here */
    double *temp_d;
    for (int i = 0; i < iterations; i++) {
	kernel<<<numBlocksInGrid, numThreadsPerBlock>>>(input_d, output_d, length);
	if (i != iterations-1) {
	    temp_d = input_d;
	    input_d = output_d;
	    output_d = temp_d;
	}
    }
    cudaThreadSynchronize();
    cudaEventRecord(comp_end);
    cudaEventSynchronize(comp_end);
    

    // copy from device to host step
    cudaEventRecord(cpy_D2H_start);
    /* Copying array from device to host goes here */  
    if (cudaMemcpy(output, output_d, length*length*sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
	cout << "error in cudaMemcpy" << endl;
    cudaEventRecord(cpy_D2H_end);
    cudaEventSynchronize(cpy_D2H_end);
    

    /* Postprocessing goes here */
    
    // cleanup
    cudaFree(input_d);
    cudaFree(output_d);
    

    float time;
    cudaEventElapsedTime(&time, cpy_H2D_start, cpy_H2D_end);
    cout<<"Host to Device MemCpy takes "<<setprecision(4)<<time/1000<<"s"<<endl;

    cudaEventElapsedTime(&time, comp_start, comp_end);
    cout<<"Computation takes "<<setprecision(4)<<time/1000<<"s"<<endl;

    cudaEventElapsedTime(&time, cpy_D2H_start, cpy_D2H_end);
    cout<<"Device to Host MemCpy takes "<<setprecision(4)<<time/1000<<"s"<<endl;
}
