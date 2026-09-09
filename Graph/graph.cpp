#include <iostream>
#include <vector>
#include<queue>
using namespace std;

struct Node {
    int id;
    string name;
    vector<int> next;
    int inDegree;
};

int main() {
    vector<Node> data;
    queue<int> ready;
    vector<string> executionState;
    data.push_back({0, "input", {1},0});
    data.push_back({1, "MatMul", {3,4},1});
    data.push_back({2, "Add", {5},2});
    data.push_back({3, "ReLu", {2},1});
    data.push_back({4, "Sigmoid", {2},1});
    data.push_back({5, "Output", {},1}); 

    //finding the indegree 0
    int i=0;
    while(!data.empty() && i<data.size()){
        if(data[i].inDegree==0){
            int val=data[i].id;
            ready.push(val);
            
        }
        i++;
    }
    //khan algorithm
    while(!ready.empty()){
        int currVal=ready.front();
        ready.pop();
        cout<<"Node: "<<data[currVal].name<<endl;
        executionState.push_back(data[currVal].name);
        for (int neighbour:data[currVal].next){
           data[neighbour].inDegree--;
           if(data[neighbour].inDegree==0){
            ready.push(neighbour);
            cout<<"Readyied :" <<data[neighbour].name<<endl;
           }
          
        }
        
    }

    //execution state verification
    for(auto &x:executionState){
        cout<<"executed : "<<x<<endl;
    }




   
}
