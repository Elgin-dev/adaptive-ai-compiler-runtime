#include "../Graph/graph.h"

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <cmath>
#include <climits>
#include <algorithm>
#include <iomanip>

using namespace std;


/* =========================================================
   CPU MATRIX MULTIPLICATION
   ========================================================= */

void cpuMatMul(
    const Tensor& input,
    Tensor& output,
    const Tensor& weights
) {

    int inputSize = input.shape.back();

    int outputSize = weights.shape.back();

    output.data.resize(outputSize);

    for (int j = 0; j < outputSize; j++) {

        float sum = 0.0f;

        for (int i = 0; i < inputSize; i++) {

            sum += input.data[i]
                 * weights.data[i * outputSize + j];
        }

        output.data[j] = sum;
    }

    output.shape = {1, outputSize};
}


/* =========================================================
   CPU RELU
   ========================================================= */

void cpuReLU(Tensor& tensor) {

    for (float& value : tensor.data) {

        if (value < 0.0f) {
            value = 0.0f;
        }
    }
}


/* =========================================================
   CPU SIGMOID
   ========================================================= */

void cpuSigmoid(Tensor& tensor) {

    for (float& value : tensor.data) {

        value = 1.0f /
                (1.0f + expf(-value));
    }
}


/* =========================================================
   NODE EXECUTION
   ========================================================= */

void executeNode(
    int id,
    vector<Node>& data,
    Tensor& tensor
) {

    data[id].state = NodeState::RUNNING;

    auto start =
        chrono::high_resolution_clock::now();

    cout << "Executing: "
         << data[id].name;

    if (data[id].backend == Backend::CUDA) {
        cout << " [CUDA]\n";
    }
    else {
        cout << " [CPU]\n";
    }


    /*
       For demonstration, the scheduler executes
       actual neural-network operations.

       We use static network dimensions.
    */

    static Tensor hidden;
    static Tensor output;

    static Tensor weights1;
    static Tensor weights2;

    static bool initialized = false;

    if (!initialized) {

        /*
           128 -> 128
        */

        weights1.shape = {128, 128};
        weights1.data.resize(128 * 128);

        /*
           128 -> 64
        */

        weights2.shape = {128, 64};
        weights2.data.resize(128 * 64);

        for (size_t i = 0; i < weights1.data.size(); i++) {

            weights1.data[i] =
                0.01f * ((i % 7) - 3);
        }

        for (size_t i = 0; i < weights2.data.size(); i++) {

            weights2.data[i] =
                0.01f * ((i % 5) - 2);
        }

        initialized = true;
    }


    if (data[id].name == "MatMul1") {

        if (data[id].backend == Backend::CUDA) {

            cudaMatMul(
                tensor,
                hidden,
                weights1
            );
        }
        else {

            cpuMatMul(
                tensor,
                hidden,
                weights1
            );
        }
    }


    else if (data[id].name == "ReLU") {

        if (data[id].backend == Backend::CUDA) {

            cudaReLU(hidden);
        }
        else {

            cpuReLU(hidden);
        }

        tensor = hidden;
    }


    else if (data[id].name == "MatMul2") {

        if (data[id].backend == Backend::CUDA) {

            cudaMatMul(
                tensor,
                output,
                weights2
            );
        }
        else {

            cpuMatMul(
                tensor,
                output,
                weights2
            );
        }

        tensor = output;
    }


    else if (data[id].name == "Sigmoid") {

        if (data[id].backend == Backend::CUDA) {

            cudaSigmoid(tensor);
        }
        else {

            cpuSigmoid(tensor);
        }
    }


    auto end =
        chrono::high_resolution_clock::now();

    auto duration =
        chrono::duration_cast<
            chrono::microseconds
        >(end - start);

    data[id].executionTime =
        duration.count();

    data[id].state =
        NodeState::COMPLETED;

    cout << "Node "
         << data[id].name
         << " completed in "
         << data[id].executionTime
         << " us\n";
}


/* =========================================================
   SCHEDULER
   ========================================================= */

void runScheduler(
    vector<Node>& data,
    Tensor& tensor
) {

    auto graphStart =
        chrono::high_resolution_clock::now();

    queue<int> ready;

    vector<int> executionOrder;


    /*
       Find initial nodes
    */

    for (int i = 0; i < data.size(); i++) {

        if (data[i].inDegree == 0) {

            data[i].state =
                NodeState::READY;

            ready.push(data[i].id);
        }
    }


    /*
       Kahn's scheduling algorithm
    */

    while (!ready.empty()) {

        vector<int> currentBatch;

        vector<thread> workers;


        /*
           Get currently ready nodes
        */

        while (!ready.empty()) {

            int node = ready.front();

            ready.pop();

            currentBatch.push_back(node);
        }


        cout << "\nReady Batch: ";

        for (int node : currentBatch) {

            cout << data[node].name
                 << " ";
        }

        cout << "\n";


        /*
           Launch workers
        */

        for (int node : currentBatch) {

            workers.emplace_back(
                executeNode,
                node,
                ref(data),
                ref(tensor)
            );

            executionOrder.push_back(node);
        }


        /*
           Wait for workers
        */

        for (auto& worker : workers) {

            worker.join();
        }


        cout << "All threads completed.\n";


        /*
           Update dependency counts
        */

        for (int node : currentBatch) {

            for (int neighbour :
                 data[node].next) {

                data[neighbour].inDegree--;


                if (data[neighbour].inDegree == 0) {

                    data[neighbour].state =
                        NodeState::READY;

                    ready.push(neighbour);

                    cout << "Readied: "
                         << data[neighbour].name
                         << "\n";
                }
            }
        }
    }


    /*
       Execution state
    */

    cout << "\nExecution State:\n";

    for (int node : executionOrder) {

        cout << "Executed: "
             << data[node].name
             << "\n";
    }


    /*
       Fastest / slowest node
    */

    long long minTime = LLONG_MAX;
    long long maxTime = LLONG_MIN;

    string fastest;
    string slowest;


    for (const auto& node : data) {

        if (node.executionTime < minTime) {

            minTime =
                node.executionTime;

            fastest =
                node.name;
        }


        if (node.executionTime > maxTime) {

            maxTime =
                node.executionTime;

            slowest =
                node.name;
        }
    }


    cout << "\nFastest Node: "
         << fastest
         << " ("
         << minTime
         << " us)\n";


    cout << "Slowest Node: "
         << slowest
         << " ("
         << maxTime
         << " us)\n";


    /*
       Critical path
    */

    vector<long long> longestTime(
        data.size(),
        0
    );


    for (int node : executionOrder) {

        long long finishTime =
            longestTime[node]
            + data[node].executionTime;


        for (int neighbour :
             data[node].next) {

            longestTime[neighbour] =
                max(
                    longestTime[neighbour],
                    finishTime
                );
        }
    }


    long long criticalPathTime = 0;

    int criticalNode = -1;


    for (int i = 0;
         i < data.size();
         i++) {

        if (longestTime[i]
            + data[i].executionTime
            > criticalPathTime) {

            criticalPathTime =
                longestTime[i]
                + data[i].executionTime;

            criticalNode = i;
        }
    }


    cout << "\nCritical Path Time: "
         << criticalPathTime
         << " us\n";


    if (criticalNode != -1) {

        cout << "Critical Path Ends At: "
             << data[criticalNode].name
             << "\n";
    }


    auto graphStop =
        chrono::high_resolution_clock::now();


    auto graphDuration =
        chrono::duration_cast<
            chrono::microseconds
        >(
            graphStop - graphStart
        );


    cout << "\nEntire Graph Duration: "
         << graphDuration.count()
         << " us\n";
}


/* =========================================================
   CPU FULL NETWORK
   ========================================================= */

void runNeuralNetworkCPU(
    const Tensor& input,
    Tensor& output
) {

    Tensor weights1;
    Tensor weights2;

    weights1.shape = {128, 128};
    weights1.data.resize(128 * 128);

    weights2.shape = {128, 64};
    weights2.data.resize(128 * 64);


    for (size_t i = 0;
         i < weights1.data.size();
         i++) {

        weights1.data[i] =
            0.01f * ((i % 7) - 3);
    }


    for (size_t i = 0;
         i < weights2.data.size();
         i++) {

        weights2.data[i] =
            0.01f * ((i % 5) - 2);
    }


    Tensor hidden;

    cpuMatMul(
        input,
        hidden,
        weights1
    );

    cpuReLU(hidden);

    cpuMatMul(
        hidden,
        output,
        weights2
    );

    cpuSigmoid(output);
}


/* =========================================================
   CUDA FULL NETWORK
   ========================================================= */

void runNeuralNetworkCUDA(
    const Tensor& input,
    Tensor& output
) {

    Tensor weights1;
    Tensor weights2;

    weights1.shape = {128, 128};
    weights1.data.resize(128 * 128);

    weights2.shape = {128, 64};
    weights2.data.resize(128 * 64);


    for (size_t i = 0;
         i < weights1.data.size();
         i++) {

        weights1.data[i] =
            0.01f * ((i % 7) - 3);
    }


    for (size_t i = 0;
         i < weights2.data.size();
         i++) {

        weights2.data[i] =
            0.01f * ((i % 5) - 2);
    }


    Tensor hidden;

    cudaMatMul(
        input,
        hidden,
        weights1
    );

    cudaReLU(hidden);

    cudaMatMul(
        hidden,
        output,
        weights2
    );

    cudaSigmoid(output);
}


/* =========================================================
   FINAL BENCHMARK
   ========================================================= */

void benchmarkNeuralNetwork() {

    Tensor input;

    input.shape = {1, 128};
    input.data.resize(128);


    for (int i = 0; i < 128; i++) {

        input.data[i] =
            (i % 10) * 0.1f;
    }


    Tensor cpuOutput;
    Tensor cudaOutput;


    const int iterations = 100;


    /*
       CPU benchmark
    */

    auto cpuStart =
        chrono::high_resolution_clock::now();


    for (int i = 0;
         i < iterations;
         i++) {

        runNeuralNetworkCPU(
            input,
            cpuOutput
        );
    }


    auto cpuEnd =
        chrono::high_resolution_clock::now();


    auto cpuTotal =
        chrono::duration_cast<
            chrono::microseconds
        >(
            cpuEnd - cpuStart
        ).count();


    /*
       CUDA warm-up
    */

    runNeuralNetworkCUDA(
        input,
        cudaOutput
    );


    /*
       CUDA benchmark
    */

    auto cudaStart =
        chrono::high_resolution_clock::now();


    for (int i = 0;
         i < iterations;
         i++) {

        runNeuralNetworkCUDA(
            input,
            cudaOutput
        );
    }


    auto cudaEnd =
        chrono::high_resolution_clock::now();


    auto cudaTotal =
        chrono::duration_cast<
            chrono::microseconds
        >(
            cudaEnd - cudaStart
        ).count();


    double cpuAverage =
        static_cast<double>(cpuTotal)
        / iterations;


    double cudaAverage =
        static_cast<double>(cudaTotal)
        / iterations;


    cout << fixed
         << setprecision(2);


    cout << "Iterations: "
         << iterations
         << "\n\n";


    cout << "CPU Total Time   : "
         << cpuTotal
         << " us\n";


    cout << "CPU Average      : "
         << cpuAverage
         << " us/inference\n\n";


    cout << "CUDA Total Time  : "
         << cudaTotal
         << " us\n";


    cout << "CUDA Average     : "
         << cudaAverage
         << " us/inference\n\n";


    /*
       Speedup
    */

    if (cudaAverage > 0) {

        double speedup =
            cpuAverage / cudaAverage;

        cout << "CUDA Speedup     : "
             << speedup
             << "x\n";
    }


    /*
       Correctness check
    */

    float maxError = 0.0f;


    if (cpuOutput.data.size()
        == cudaOutput.data.size()) {

        for (size_t i = 0;
             i < cpuOutput.data.size();
             i++) {

            float error =
                fabs(
                    cpuOutput.data[i]
                    -
                    cudaOutput.data[i]
                );

            maxError =
                max(maxError, error);
        }
    }


    cout << "\nMaximum CPU/CUDA Error: "
         << maxError
         << "\n";


    if (maxError < 0.001f) {

        cout << "Correctness: PASS\n";
    }
    else {

        cout << "Correctness: CHECK REQUIRED\n";
    }
}