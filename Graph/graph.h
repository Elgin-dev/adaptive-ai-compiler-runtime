#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

enum class NodeState {
    WAITING,
    READY,
    RUNNING,
    COMPLETED
};

enum class Backend {
    CPU,
    CUDA
};

struct Tensor {
    std::vector<float> data;
    std::vector<int> shape;
};

struct Node {
    int id;
    std::string name;
    std::vector<int> next;
    int inDegree;
    NodeState state;
    long long executionTime;
    Backend backend;
};

void runScheduler(std::vector<Node>& data, Tensor& tensor);

void executeNode(
    int id,
    std::vector<Node>& data,
    Tensor& tensor
);

// CPU operations
void cpuMatMul(
    const Tensor& input,
    Tensor& output,
    const Tensor& weights
);

void cpuReLU(Tensor& tensor);

void cpuSigmoid(Tensor& tensor);

// CUDA operations
void cudaMatMul(
    const Tensor& input,
    Tensor& output,
    const Tensor& weights
);

void cudaReLU(Tensor& tensor);

void cudaSigmoid(Tensor& tensor);

// Neural network
void runNeuralNetworkCPU(
    const Tensor& input,
    Tensor& output
);

void runNeuralNetworkCUDA(
    const Tensor& input,
    Tensor& output
);

// Benchmark
void benchmarkNeuralNetwork();

#endif