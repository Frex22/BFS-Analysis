#pragma once
#include "graph.h"
#include <vector>
/**
 * Sequential BFS implementation
 * 
 * @param graph The input graph in CSR format
 * @param source The source vertex to start BFS from
 * @return Vector of distances from source to each vertex (INT_MAX if unreachable)
 */
std::vector<int> bfsSequential(const Graph& graph, int source);

