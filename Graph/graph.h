#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>

enum class NodeState {
    WAITING,
    READY,
    RUNNING,
    COMPLETED
};
struct Node {
    int id;
    std::string name;
    std::vector<int> next;
    int inDegree;
    NodeState state;
};
void runScheduler(std::vector<Node>& data);
void executeNode(int id, std::vector<Node>& data);

#endif