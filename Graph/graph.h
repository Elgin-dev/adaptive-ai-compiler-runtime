#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

struct Node {
    int id;
    std::string name;
    std::vector<int> next;
    int inDegree;
};
void runScheduler(std::vector<Node>& data);
void executeNode(int id, std::vector<Node>& data);

#endif