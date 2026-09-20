#include "../Graph/graph.h"

#include <cuda_runtime.h>

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;


/* =========================================================
   CUDA ERROR CHECK
   ========================================================= */

#define CUDA_CHECK(call)                                      \
    do {                                                       \
        cudaError_t error = call;                             \
        if (error != cudaSuccess) {                           \
            cerr << "CUDA Error: "                             \
                 << cudaGetErrorString(error)                \
                 << endl;                                     \
            exit(EXIT_FAILURE);                               \
        }                                                      \
    } while (0)


/* =========================================================
   CUDA MATRIX MULTIPLICATION KERNEL
   =========================================================

   Input:
       1 x N

   Weight:
       N x M

   Output:
       1 x M
   ========================================================= */

__global__
void matMulKernel(
    const float* input,
    const float* weights,
    float* output,
    int inputSize,
    int outputSize
) {

    int j =
        blockIdx.x * blockDim.x
        + threadIdx.x;


    if (j < outputSize) {

        float sum = 0.0f;


        for (int i = 0;
             i < inputSize;
             i++) {

            sum +=
                input[i]
                *
                weights[
                    i * outputSize + j
                ];
        }


        output[j] = sum;
    }
}


/* =========================================================
   CUDA RELU
   ========================================================= */

__global__
void reluKernel(
    float* data,
    int size
) {

    int index =
        blockIdx.x * blockDim.x
        + threadIdx.x;


    if (index < size) {

        if (data[index] < 0.0f) {

            data[index] = 0.0f;
        }
    }
}


/* =========================================================
   CUDA SIGMOID
   ========================================================= */

__global__
void sigmoidKernel(
    float* data,
    int size
) {

    int index =
        blockIdx.x * blockDim.x
        + threadIdx.x;


    if (index < size) {

        data[index] =
            1.0f /
            (1.0f + expf(-data[index]));
    }
}


/* =========================================================
   CUDA MATMUL HOST FUNCTION
   ========================================================= */

void cudaMatMul(
    const Tensor& input,
    Tensor& output,
    const Tensor& weights
) {

    int inputSize =
        input.shape.back();


    int outputSize =
        weights.shape.back();


    size_t inputBytes =
        inputSize * sizeof(float);


    size_t weightBytes =
        inputSize
        * outputSize
        * sizeof(float);


    size_t outputBytes =
        outputSize
        * sizeof(float);


    float* d_input = nullptr;

    float* d_weights = nullptr;

    float* d_output = nullptr;


    CUDA_CHECK(
        cudaMalloc(
            &d_input,
            inputBytes
        )
    );


    CUDA_CHECK(
        cudaMalloc(
            &d_weights,
            weightBytes
        )
    );


    CUDA_CHECK(
        cudaMalloc(
            &d_output,
            outputBytes
        )
    );


    CUDA_CHECK(
        cudaMemcpy(
            d_input,
            input.data.data(),
            inputBytes,
            cudaMemcpyHostToDevice
        )
    );


    CUDA_CHECK(
        cudaMemcpy(
            d_weights,
            weights.data.data(),
            weightBytes,
            cudaMemcpyHostToDevice
        )
    );


    int threads = 256;


    int blocks =
        (outputSize + threads - 1)
        / threads;


    matMulKernel<<<blocks, threads>>>(
        d_input,
        d_weights,
        d_output,
        inputSize,
        outputSize
    );


    CUDA_CHECK(
        cudaGetLastError()
    );


    CUDA_CHECK(
        cudaDeviceSynchronize()
    );


    output.data.resize(
        outputSize
    );


    output.shape = {
        1,
        outputSize
    };


    CUDA_CHECK(
        cudaMemcpy(
            output.data.data(),
            d_output,
            outputBytes,
            cudaMemcpyDeviceToHost
        )
    );


    CUDA_CHECK(
        cudaFree(d_input)
    );


    CUDA_CHECK(
        cudaFree(d_weights)
    );


    CUDA_CHECK(
        cudaFree(d_output)
    );
}


/* =========================================================
   CUDA RELU HOST FUNCTION
   ========================================================= */

void cudaReLU(
    Tensor& tensor
) {

    int size =
        static_cast<int>(
            tensor.data.size()
        );


    float* d_data = nullptr;


    CUDA_CHECK(
        cudaMalloc(
            &d_data,
            size * sizeof(float)
        )
    );


    CUDA_CHECK(
        cudaMemcpy(
            d_data,
            tensor.data.data(),
            size * sizeof(float),
            cudaMemcpyHostToDevice
        )
    );


    int threads = 256;


    int blocks =
        (size + threads - 1)
        / threads;


    reluKernel<<<blocks, threads>>>(
        d_data,
        size
    );


    CUDA_CHECK(
        cudaGetLastError()
    );


    CUDA_CHECK(
        cudaDeviceSynchronize()
    );


    CUDA_CHECK(
        cudaMemcpy(
            tensor.data.data(),
            d_data,
            size * sizeof(float),
            cudaMemcpyDeviceToHost
        )
    );


    CUDA_CHECK(
        cudaFree(d_data)
    );
}


/* =========================================================
   CUDA SIGMOID HOST FUNCTION
   ========================================================= */

void cudaSigmoid(
    Tensor& tensor
) {

    int size =
        static_cast<int>(
            tensor.data.size()
        );


    float* d_data = nullptr;


    CUDA_CHECK(
        cudaMalloc(
            &d_data,
            size * sizeof(float)
        )
    );


    CUDA_CHECK(
        cudaMemcpy(
            d_data,
            tensor.data.data(),
            size * sizeof(float),
            cudaMemcpyHostToDevice
        )
    );


    int threads = 256;


    int blocks =
        (size + threads - 1)
        / threads;


    sigmoidKernel<<<blocks, threads>>>(
        d_data,
        size
    );


    CUDA_CHECK(
        cudaGetLastError()
    );


    CUDA_CHECK(
        cudaDeviceSynchronize()
    );


    CUDA_CHECK(
        cudaMemcpy(
            tensor.data.data(),
            d_data,
            size * sizeof(float),
            cudaMemcpyDeviceToHost
        )
    );


    CUDA_CHECK(
        cudaFree(d_data)
    );
}