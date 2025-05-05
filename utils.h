#pragma once

#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
/**
 * Simple timer class for performance measurements
 */
class Timer {
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    // Returns elapsed time in milliseconds
    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(
            end_time - start_time).count();
    }
    
    // Print elapsed time with a label
    void printElapsed(const std::string& label = "") const {
        std::cout << label << elapsed() << " ms" << std::endl;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time;
};

/**
 * Performance stats structure
 */
struct BFSStats {
    int source;
    double time_ms;
    int num_vertices;
    int num_edges;
    int num_visited;
    int max_distance;
    std::string implementation;
    
    void print() const {
        std::cout << "BFS Performance (" << implementation << "):" << std::endl;
        std::cout << "  Source vertex: " << source << std::endl;
        std::cout << "  Execution time: " << time_ms << " ms" << std::endl;
        std::cout << "  Vertices visited: " << num_visited << "/" << num_vertices 
                  << " (" << std::fixed << std::setprecision(2) 
                  << (100.0 * num_visited / num_vertices) << "%)" << std::endl;
        std::cout << "  Maximum distance: " << max_distance << std::endl;
        std::cout << "  Edges traversed: " << num_edges << std::endl;
        std::cout << "  MTEPS: " << std::fixed << std::setprecision(2) 
                  << (num_edges / (time_ms / 1000.0)) / 1e6 << std::endl;
    }
};

// Function declaration (not definition)
BFSStats calculateBFSStats(const std::vector<int>& distances, int source, 
                          double time_ms, int num_vertices, int num_edges, 
                          const std::string& implementation = "Sequential");