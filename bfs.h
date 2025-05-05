#pragma once
#include "graph.h"  // Make sure this include path is correct
#include <vector>

/**
 * Sequential BFS implementation
 * 
 * @param graph The input graph in CSR format
 * @param source The source vertex to start BFS from
 * @return Vector of distances from source to each vertex (INT_MAX if unreachable)
 */
std::vector<int> bfsSequential(const Graph& graph, int source);


/**
 * OpenMP parallel BFS implementation
 * 
 * @param graph The input graph in CSR format
 * @param source The source vertex to start BFS from
 * @param num_threads Number of threads to use for parallel execution
 * @return Vector of distances from source to each vertex (INT_MAX if unreachable)
 */
std::vector<int> bfsOpenMP(const Graph& graph, int source, int num_threads = 0);

/**
 * Verifies that two BFS results match
 * 
 * @param result The BFS result to verify
 * @param reference The reference BFS result to compare against
 * @return True if the results match, false otherwise
 */
bool verifyBFS(const std::vector<int>& result, const std::vector<int>& reference);
