#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include "../Graph/graph.h"

using namespace std;



void executeNode(int id,vector<Node>& data){
    cout<<"Executing : "<<data[id].name<<endl;
}

int main() {
    vector<Node> data;

    data.push_back({0, "input", {1}, 0});
    data.push_back({1, "MatMul", {3, 4}, 1});
    data.push_back({2, "Add", {5}, 2});
    data.push_back({3, "ReLu", {2}, 1});
    data.push_back({4, "Sigmoid", {2}, 1});
    data.push_back({5, "Output", {}, 1});

    runScheduler(data);

    
    return 0;
}
