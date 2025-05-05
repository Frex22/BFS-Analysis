#pragma once
#include <vector>
#include<string>

/*Graph rep using CSR*/

class Graph {
public:

    // load graph from file
    Graph() : num_vertices(0), num_edges(0) {}
    static Graph loadFromFile(const std::string& filename, bool directed = true);

    // Basic Properties
    int getNumVertices() const { return num_vertices; }
    int getNumEdges() const { return num_edges; }

    //access methods
    int getNeighborCount(int vertex) const {
        return row_offsets[vertex + 1] - row_offsets[vertex];
    }

    //Returns the range of neghibor indices for a given vertex
    std::pair<int, int> getNeighborRange(int vertex) const {
        return { row_offsets[vertex], row_offsets[vertex + 1] };
    }

    //GEt neighbor at a specific eddge index
    int getNeighbor(int edge_idx) const {
        return column_indices[edge_idx];
    }

    void printStats() const ;

private:
    int num_vertices;
    int num_edges;

    std::vector<int> row_offsets; // size: num_vertices + 1
    std::vector<int> column_indices; // size: num_edges

    // For directed graphs, we need to store the reverse graph as well
    std::vector<int> reverse_row_offsets;
    std::vector<int> reverse_column_indices;

    // For undirected graphs, we need to store the undirected edges
    
    // Make BFS functions friends so they can access private members
    friend std::vector<int> bfsSequential(const Graph& graph, int source);
    friend std::vector<int> bfsOpenMP(const Graph& graph, int source, int num_threads);
    // Future CUDA implementation will also be a friend
};
