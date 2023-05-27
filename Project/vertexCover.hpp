#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <minisat/core/Solver.h>
#include <pthread.h>
#include <chrono>
#include <thread>

class VertexCover
{

public:
    VertexCover();
    int vertex;
    std::vector<std::pair<int, int>> edges;
    void addvertex(int x);
    void addEdges(std::vector<std::pair<int, int>> &s);
    void printVertexCover();
    void reset();
};