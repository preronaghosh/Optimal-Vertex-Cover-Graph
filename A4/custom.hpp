#include <iostream>
#include <vector>
#include <minisat/core/SolverTypes.h>
#include <minisat/core/Solver.h>
using namespace std;

class GraphVertexCover
{
public:
    int vertices;
    vector<pair<int, int>> edge_list;

    GraphVertexCover(int V) {vertices=V;}
    void setNumVertices(int V) {vertices=V;}
    int getNumVertices() { return vertices; }
    vector<int> get_vertex_cover();
};