#include "../Graph/graph.h"
#include <iostream>
#include <vector>
#include <queue>
#include <thread>


using namespace std;

void runScheduler(vector<Node>& data) {

    queue<int> ready;
    vector<string> executionState;


    // Find nodes with indegree 0
    for (int i = 0; i < data.size(); i++) {
        if (data[i].inDegree == 0) {
            data[i].state=NodeState::READY;
            ready.push(data[i].id);
        }
    }

    // Kahn's scheduling
    while (!ready.empty()) {

        vector<int> currentBatch;
        vector<thread> workers;

        // Take all currently ready nodes
        while (!ready.empty()) {
            int node = ready.front();
            ready.pop();

            currentBatch.push_back(node);
        }

        cout << "\nReady Batch: ";

        for (int node : currentBatch) {
            cout << data[node].name << " ";
        }

        cout << endl;

        // Create threads dynamically
        for (int node : currentBatch) {

            cout << "Creating thread for: "
                 << data[node].name << endl;

            workers.push_back(
                thread(executeNode, node, ref(data))
            );

            executionState.push_back(data[node].name);
        }

        // Wait for all threads to finish
        for (auto& worker : workers) {
            worker.join();
        }

        cout << "All threads completed.\n";

        // Update dependencies AFTER execution
        for (int node : currentBatch) {

            for (int neighbour : data[node].next) {

                data[neighbour].inDegree--;

                if (data[neighbour].inDegree == 0) {
                     data[neighbour].state=NodeState::READY;
                    ready.push(neighbour);

                    cout << "Readied: "
                         << data[neighbour].name
                         << endl;
                }
            }
        }
    }

    // Execution state
    cout << "\nExecution State:\n";

    for (auto& x : executionState) {
        cout << "Executed: " << x << endl;
    }
}