#include <iostream>
#include <vector>
using namespace std;

struct Node {
    int id;
    string name;
    vector<int> next;
};

int main() {
    vector<Node> data;

    data.push_back({0, "input", {1}});
    data.push_back({1, "MatMul", {2}});
    data.push_back({2, "Add", {3}});
    data.push_back({3, "ReLu", {4}});
    data.push_back({4, "Output", {-1}}); 

    int i=0;
    while(!data.empty() && i<data.size()){
        cout<<"Executing Node: "<<data[i].id<<"| "<<data[i].name<<endl;
        i=data[i].next[0];
    }

   /* for (auto &node : data) {
        cout << "Node " << node.id << " -> " << node.name;
        if (!node.next.empty()) {
        cout << " | next: ";
        for (int n : node.next) {
            cout << n << " ";
        }
    }
       cout<<endl; 
    }*/
}
