#include "graph.h"
#include "bfs.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <random>
#include <omp.h>  // Add OpenMP header

// Calculate BFS statistics

BFSStats calculateBFSStats(const std::vector<int>& distances, int source, 
                           double time_ms, int num_vertices, int num_edges, 
                           const std::string& implementation) {

    //count number of visited vertices
    int visited = 0;
    int max_distance = -1;

    for (int d : distances) {

        if (d != std::numeric_limits<int>::max()){ // If the vertex is reachable 
            visited++;
            max_distance = std::max(max_distance, d);
        }
    }

    return BFSStats{
        source,
        time_ms,
        num_vertices,
        num_edges,
        visited,
        max_distance,
        implementation
    };
   
}


int main (int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> <source_vertex> [directed=1] [num_runs=1] [num_threads=4]" << std::endl;
        return 1;
    }

    std::string graph_file = argv[1];
    int source = std::stoi(argv[2]);
    bool directed = ( argc > 3) ? std::stoi(argv[3]) !=0 : true; // Default to directed graph
    int num_runs = (argc > 4) ? std::stoi(argv[4]) : 1; // Default to 1 run
    int num_threads = (argc > 5) ? std::stoi(argv[5]) : 4; // Default to 4 threads

    // Load the graph from the file
    std::cout << "Loading graph from file: " << graph_file << std::endl;
    Graph graph = Graph::loadFromFile(graph_file, directed);

    //Print Graph stats
    graph.printStats();

    //Validate source vertex
    if (source < 0 || source >= graph.getNumVertices()) {
        std::cerr << "Error: Source vertex " << source << " out of range [0, " 
                  << graph.getNumVertices() - 1 << "]" << std::endl;
        
        // Choose a random valid source instead
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, graph.getNumVertices() - 1);
        source = distrib(gen);
        std::cout << "Randomly selected source vertex: " << source << std::endl;
    }

    // Run Sequential BFS
    std::cout << "\nRunning Sequential BFS..." << std::endl;
    Timer timer;
    std::vector<int> reference_distances;
    double total_seq_time = 0.0;
    
    for (int run = 0; run < num_runs; run++) {
        timer.start();
        auto distances = bfsSequential(graph, source);
        double elapsed_ms = timer.elapsed();
        total_seq_time += elapsed_ms;
        
        if (run == 0) {
            reference_distances = distances;
            auto stats = calculateBFSStats(
                distances, source, elapsed_ms, 
                graph.getNumVertices(), graph.getNumEdges(), "Sequential"
            );
            stats.print();
        } else {
            std::cout << "Run " << run + 1 << ": " << elapsed_ms << " ms" << std::endl;
        }
    }
    
    if (num_runs > 1) {
        std::cout << "Average Sequential BFS time: " << (total_seq_time / num_runs) << " ms" << std::endl;
    }
    
    // Run OpenMP BFS
    std::cout << "\nRunning OpenMP BFS with " << num_threads << " threads..." << std::endl;
    double total_omp_time = 0.0;
    
    for (int run = 0; run < num_runs; run++) {
        timer.start();
        auto distances = bfsOpenMP(graph, source, num_threads);
        double elapsed_ms = timer.elapsed();
        total_omp_time += elapsed_ms;
        
        if (run == 0) {
            // Verify results match sequential version
            bool correct = verifyBFS(distances, reference_distances);
            
            if (!correct) {
                std::cerr << "ERROR: OpenMP BFS results do not match sequential results!" << std::endl;
            } else {
                std::cout << "OpenMP results verified correct." << std::endl;
            }
            
            auto stats = calculateBFSStats(
                distances, source, elapsed_ms, 
                graph.getNumVertices(), graph.getNumEdges(), "OpenMP"
            );
            stats.print();
            
            // Calculate and display speedup
            double speedup = total_seq_time / elapsed_ms;
            std::cout << "Speedup over sequential: " << speedup << "x" << std::endl;
        } else {
            std::cout << "Run " << run + 1 << ": " << elapsed_ms << " ms" << std::endl;
        }
    }
    
    if (num_runs > 1) {
        std::cout << "Average OpenMP BFS time: " << (total_omp_time / num_runs) << " ms" << std::endl;
        std::cout << "Average speedup: " << (total_seq_time / total_omp_time) << "x" << std::endl;
    }
    
    return 0;
}
