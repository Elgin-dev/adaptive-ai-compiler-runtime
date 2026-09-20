#include <iostream>
#include <cuda_runtime.h>

using namespace std;

__global__ void reluKernel(float* data, int N)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x;

    if (id < N) {
        if (data[id] < 0) {
            data[id] = 0;
        }
    }
}

int main()
{
    const int N = 8;

    float h_data[N] = {
        -2.0f, 4.0f, -1.0f, 7.0f,
        -5.0f, 3.0f, -8.0f, 6.0f
    };

    float* d_data;

    // Allocate GPU memory
    cudaMalloc(&d_data, N * sizeof(float));

    // CPU → GPU
    cudaMemcpy(
        d_data,
        h_data,
        N * sizeof(float),
        cudaMemcpyHostToDevice
    );

    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1)
                        / threadsPerBlock;

    cout << "Blocks: " << blocksPerGrid << endl;
    cout << "Threads per block: "
         << threadsPerBlock << endl;

    // Launch CUDA kernel
    reluKernel<<<blocksPerGrid, threadsPerBlock>>>(d_data, N);

    cudaDeviceSynchronize();

    // GPU → CPU
    cudaMemcpy(
        h_data,
        d_data,
        N * sizeof(float),
        cudaMemcpyDeviceToHost
    );

    cout << "ReLU Output: ";

    for (int i = 0; i < N; i++) {
        cout << h_data[i] << " ";
    }

    cout << endl;

    cudaFree(d_data);

    return 0;
}