#include "graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

Graph Graph::loadFromFile(const std::string& filename, bool directed) {
    Graph graph;
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Error opening file: " << filename << std::endl;
        return graph;
    }

    //First pass: read edges and identify vertices
    
    std::unordered_map<int, std::vector<int>> adj_list;
    int max_vertex_id = -1; // To track the maximum vertex ID initially there is no vertex hence -1

    std::string line; // To store each line of the file
    // Read the file line by line

    while(std::getline(file, line)){
        if(line.empty() || line[0] == '#') continue; // Skip empty lines and comments

        std::istringstream iss (line); // Create a string stream from the line
        int src, dest;
        if(!(iss >> src >> dest)) continue; // Read source and destination vertices

        adj_list[src].push_back(dest); // Add destination to the adjacency list of source
        if(!directed) {
            adj_list[dest].push_back(src); // For undirected graphs, add the reverse edge
        }
        max_vertex_id = std::max({max_vertex_id, src, dest}); // Update the maximum vertex ID
    }

    //set number of vertices and prepare for CSR format
    graph.num_vertices = max_vertex_id + 1; // Number of vertices is max ID + 1
    graph.row_offsets.resize(graph.num_vertices + 1, 0); // Resize row offsets to num_vertices + 1

    //second pass: count edges and fill row offsets, Build CSR format
    for (const auto& [vertex, neighbors] : adj_list) {
        graph.row_offsets[vertex + 1] = neighbors.size(); // Set the size of the row for this vertex
    }

    //compute sum of row offsets
    for(int i =1; i <= graph.num_vertices; i++){
        graph.row_offsets[i] += graph.row_offsets[i - 1]; // Cumulative sum to get the offsets
    }
   
    //allocate column indices and fill them
    graph.num_edges = graph.row_offsets[graph.num_vertices]; // Total number of edges
    graph.column_indices.resize(graph.num_edges); // Resize column indices to num_edges
    std::vector<int> edge_counters(graph.num_vertices, 0); // To track the number of edges for each vertex
    for(const auto& [vertex, neighbors] : adj_list){
        for (int neighbor : neighbors){
            graph.column_indices[edge_counters[vertex]++] = neighbor; // Fill the column indices  
            
        }
    }

    //sort neighbors for each vertex
    for (int i = 0; i < graph.num_vertices; i++){
        int start = graph.row_offsets[i];
        int end = graph.row_offsets[i + 1];
        std::sort(graph.column_indices.begin() + start, graph.column_indices.begin() + end); // Sort the neighbors
    }
    return graph;
}

void Graph::printStats() const {
    std::cout << "Graph Statistics:" << std::endl;
    std::cout << "  Vertices: " << num_vertices << std::endl;
    std::cout << "  Edges: " << num_edges << std::endl;
    
    // Calculate average degree
    double avg_degree = static_cast<double>(num_edges) / num_vertices;
    std::cout << "  Average degree: " << avg_degree << std::endl;
    
    // Find max degree
    int max_degree = 0;
    for (int i = 0; i < num_vertices; i++) {
        int degree = row_offsets[i + 1] - row_offsets[i];
        max_degree = std::max(max_degree, degree);
    }
    std::cout << "  Maximum degree: " << max_degree << std::endl;
}
