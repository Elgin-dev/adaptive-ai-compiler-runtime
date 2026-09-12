#include "../Graph/graph.h"
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <climits>
#include <chrono>


using namespace std;

void runScheduler(vector<Node>& data) {

    auto graphStart = chrono::high_resolution_clock::now();
    queue<int> ready;
    vector<string> executionState;
    vector<int> executionOrder;
    vector<long long> longestTime(data.size(), 0);
    long maxTime=LONG_MIN;
    long minTime=LONG_MAX;
    string slowNode;
    string fastNode;
    long long criticalPathTime = 0;
    int criticalNode = -1;


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
            executionOrder.push_back(node);
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
    

    //time calculation for best and worst Nodes
    //for maximum and minimum
    for(int i = 0; i < data.size(); i++){

    if(data[i].executionTime > maxTime){
        maxTime = data[i].executionTime;
        slowNode = data[i].name;
    }

    if(data[i].executionTime < minTime){
        minTime = data[i].executionTime;
        fastNode = data[i].name;
    }
}

    cout << "Fastest Node: " << fastNode << endl;
    cout << "Fastest Node Time: " << minTime << " us" << endl;

    cout << "Slowest Node: " << slowNode << endl;
    cout << "Slowest Node Time: " << maxTime << " us" << endl;


    //
    for (int node : executionOrder) {

    long long currentTime =
        longestTime[node] + data[node].executionTime;

    for (int neighbour : data[node].next) {

        if (currentTime > longestTime[neighbour]) {
            longestTime[neighbour] = currentTime;
        }

    }
}

    //critical-path node finding
    for (int i = 0; i < data.size(); i++) {

    if (longestTime[i] > criticalPathTime) {
        criticalPathTime = longestTime[i];
        criticalNode = i;
    }

}
cout << "Critical Path Time: "
     << criticalPathTime << " us" << endl;

cout << "Critical Path Ends At: "
     << data[criticalNode].name << endl;
    auto graphStop = chrono::high_resolution_clock::now();
    auto graphDuration=std::chrono::duration_cast<chrono::microseconds>(graphStop-graphStart);
    cout<< "Entire Graph Duration is : "<<graphDuration.count()<<" us"<<endl;
}