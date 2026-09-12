#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include "../Graph/graph.h"

using namespace std;



void executeNode(int id, vector<Node>& data) {

    data[id].state = NodeState::RUNNING;

    auto start = chrono::high_resolution_clock::now();

    cout << "Executing : "
         << data[id].name << endl;


    auto end = chrono::high_resolution_clock::now();

    auto duration =
        chrono::duration_cast<chrono::microseconds>(end - start);

    data[id].executionTime = duration.count();
    data[id].state = NodeState::COMPLETED;
    cout << "Node "
         << data[id].name
         << " completed in "
         << data[id].executionTime
         << " us"
         << endl;
}

void relu(Tensor& input)
{
    for (int i = 0; i < input.data.size(); i++) {
        if (input.data[i] < 0) {
            input.data[i] = 0;
        }
    }

    cout<<"values"<<endl;
    for(auto &x: input.data){
    cout<<x<< " "; 
}
}

int main() {
    vector<Node> data;
    Tensor values;
    values.data = {-2.0, 4.0, -1.0, 7.0};
    data.push_back({0, "input", {1}, 0, NodeState::WAITING, 0});
    data.push_back({1, "MatMul", {3, 4}, 1, NodeState::WAITING, 0});
    data.push_back({2, "Add", {5}, 2, NodeState::WAITING, 0});
    data.push_back({3, "ReLu", {2}, 1, NodeState::WAITING, 0});
    data.push_back({4, "Sigmoid", {2}, 1, NodeState::WAITING, 0});
    data.push_back({5, "Output", {}, 1, NodeState::WAITING, 0}); 

    runScheduler(data);
    relu(values);

    
    return 0;
}
