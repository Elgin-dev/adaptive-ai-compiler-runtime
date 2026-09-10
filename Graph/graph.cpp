#include "../Graph/graph.h"
#include <iostream>
#include <vector>
#include <queue>
using namespace std;



void runScheduler(vector<Node>& data) {
    queue<int> ready;
    vector<string> executionState;

    // find indegree 0
    for (int i = 0; i < data.size(); i++) {
        if (data[i].inDegree == 0) {
            ready.push(data[i].id);
        }
    }

    // Kahn’s algorithm
    while (!ready.empty()) {
        
         if (ready.size() > 1) {
        cout << "Parallel work available: "
             << ready.size() << " nodes" << endl;
    }

        int currVal = ready.front();
        ready.pop();
        cout << "Node: " << data[currVal].name << endl;
        executionState.push_back(data[currVal].name);

        for (int neighbour : data[currVal].next) {
            data[neighbour].inDegree--;
            if (data[neighbour].inDegree == 0) {
                ready.push(neighbour);
                cout << "Readied: " << data[neighbour].name << endl;
            }
        }
    }

    // execution state verification
    for (auto& x : executionState) {
        cout << "executed : " << x << endl;
    }
}
