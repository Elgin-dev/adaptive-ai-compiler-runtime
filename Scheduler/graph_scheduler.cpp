#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

#include "../Graph/graph.h"

using namespace std;

int main() {

    cout << "========================================\n";
    cout << "   AI COMPILER - NEURAL NETWORK ENGINE\n";
    cout << "========================================\n\n";

    /*
        Neural Network:

        Input
          |
        MatMul1
          |
         ReLU
          |
        MatMul2
          |
       Sigmoid
          |
        Output
    */

    vector<Node> data;

    data.push_back({
        0,
        "Input",
        {1},
        0,
        NodeState::WAITING,
        0,
        Backend::CPU
    });

    data.push_back({
        1,
        "MatMul1",
        {2},
        1,
        NodeState::WAITING,
        0,
        Backend::CUDA
    });

    data.push_back({
        2,
        "ReLU",
        {3},
        1,
        NodeState::WAITING,
        0,
        Backend::CUDA
    });

    data.push_back({
        3,
        "MatMul2",
        {4},
        1,
        NodeState::WAITING,
        0,
        Backend::CUDA
    });

    data.push_back({
        4,
        "Sigmoid",
        {5},
        1,
        NodeState::WAITING,
        0,
        Backend::CPU
    });

    data.push_back({
        5,
        "Output",
        {},
        1,
        NodeState::WAITING,
        0,
        Backend::CPU
    });

    /*
        Input tensor

        1 x 128
    */

    Tensor tensor;

    tensor.shape = {1, 128};
    tensor.data.resize(128);

    for (int i = 0; i < 128; i++) {
        tensor.data[i] = (i % 10) * 0.1f;
    }

    cout << "Network: 128 -> 128 -> 64\n";
    cout << "MatMul1 : CUDA\n";
    cout << "ReLU    : CUDA\n";
    cout << "MatMul2 : CUDA\n";
    cout << "Sigmoid : CPU\n\n";

    runScheduler(data, tensor);

    cout << "\n========================================\n";
    cout << "        FINAL BENCHMARK\n";
    cout << "========================================\n\n";

    benchmarkNeuralNetwork();

    return 0;
}