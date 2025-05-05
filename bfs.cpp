#include "bfs.h"  // Include this first to ensure proper declaration
#include <queue>
#include <limits>
#include <iostream>
#include <omp.h>  // Include OpenMP header
#include <vector>

// Sequential BFS implementation
std::vector<int> bfsSequential(const Graph& graph, int source) {
    const int num_vertices = graph.getNumVertices();

    // Initialize distances to all vertices as infinity
    std::vector<int> distances(num_vertices, std::numeric_limits<int>::max());
    distances[source] = 0; // Distance to source is 0

    // Queue for bfs
    std::queue<int> queue;
    queue.push(source); // Start BFS from the source vertex

    // BFS loop
    while(!queue.empty()) {
        int current_vertex = queue.front();
        queue.pop();

        // Process all neighbors
        auto [start, end] = graph.getNeighborRange(current_vertex);
        for (int edge_idx = start; edge_idx < end; edge_idx++) {
            int neighbor = graph.getNeighbor(edge_idx); // Get the neighbor vertex
            // If the neighbor has not been visited
            if (distances[neighbor] == std::numeric_limits<int>::max()) {
                distances[neighbor] = distances[current_vertex] + 1; // Update distance
                queue.push(neighbor); // Add neighbor to the queue for further processing
            }
        }
    }

    return distances; // Return the distances from the source to all vertices
}

// OpenMP parallel BFS implementation
std::vector<int> bfsOpenMP(const Graph& graph, int source, int num_threads) {
    const int num_vertices = graph.getNumVertices();
    
    // Set number of threads if specified
    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }
    
    // Initialize distances to "infinity"
    std::vector<int> distances(num_vertices, std::numeric_limits<int>::max());
    distances[source] = 0;
    
    // BFS uses level-synchronous approach with two frontiers
    std::vector<int> current_frontier;
    std::vector<int> next_frontier;
    
    // Initial frontier contains just the source
    current_frontier.push_back(source);
    
    // Current BFS level
    int level = 1;
    
    // Process each level of BFS
    while (!current_frontier.empty()) {
        next_frontier.clear();
        
        // Process current frontier in parallel
        #pragma omp parallel
        {
            // Thread-local storage for discovered vertices
            std::vector<int> local_frontier;
            
            // Divide frontier vertices among threads
            #pragma omp for schedule(dynamic, 64)
            for (size_t i = 0; i < current_frontier.size(); i++) {
                int vertex = current_frontier[i];
                
                // Get neighbors of this vertex
                auto [start, end] = graph.getNeighborRange(vertex);
                
                // Process all neighbors
                for (int edge_idx = start; edge_idx < end; edge_idx++) {
                    int neighbor = graph.getNeighbor(edge_idx);
                    
                    // Check if neighbor hasn't been visited yet
                    if (distances[neighbor] == std::numeric_limits<int>::max()) {
                        // Use critical section for correctness
                        #pragma omp critical
                        {
                            // Check again in case another thread updated it
                            if (distances[neighbor] == std::numeric_limits<int>::max()) {
                                distances[neighbor] = level;
                                local_frontier.push_back(neighbor);
                            }
                        }
                    }
                }
            }
            
            // Combine thread-local frontiers into the next global frontier
            #pragma omp critical
            {
                next_frontier.insert(next_frontier.end(), local_frontier.begin(), local_frontier.end());
            }
        } // End parallel region
        
        // Move to next level
        current_frontier.swap(next_frontier);
        level++;
    }
    
    return distances;
}

// BFS result verification
bool verifyBFS(const std::vector<int>& result, const std::vector<int>& reference) {
    if (result.size() != reference.size()) {
        std::cerr << "Result and reference sizes do not match." << std::endl;
        return false;
    }

    for (size_t i = 0; i < result.size(); i++) {
        if(result[i] != reference[i]) {
            std::cout << "Error at vertex " << i
                     << ": got " << result[i]
                     << " expected " << reference[i] << std::endl;
            return false;
        }
    }

    return true;
}