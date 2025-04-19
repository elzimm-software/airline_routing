
#ifndef AIRLINE_ROUTING_TREE_H
#define AIRLINE_ROUTING_TREE_H

#include <string>
#include <vector>
#include <limits>
#include <functional>
#include "graph.h"
#include "map.h"

using std::vector;
using std::string;
using UndirectedGraph = Graph::UndirectedGraph;

// Minimal Spanning Tree
class Tree {
    Map<string, int> edges;

public:

    // Applies Prim's algorithm to generate a MST from an UndirectedGraph
    void prim_mst(const UndirectedGraph& ug) {
        // Clear existing edges
        edges.clear();
        const auto& graph_edges = ug.get_edges();
        if (graph_edges.empty()) return;
        Map<string, bool> in_mst;
        Map<string, int> key;
        Map<string, string> parent;

        vector<string> vertices;
        // Iterate over edges and initialize maps
        for (const auto& [vertex, _]: graph_edges) {
            vertices.push_back(vertex);
            key[vertex] = std::numeric_limits<int>::max();
            in_mst[vertex] = false;
        }

        // Start at the first vertex
        string start = vertices[0];
        key[start] = 0;

        // Iterate over vertices
        for (size_t count = 0; count < vertices.size(); count++) {
            string u;
            int min_cost = std::numeric_limits<int>::max();
            // Find lowest cost node not in tree
            for (const auto& v: vertices) {
                if (!in_mst[v] && key[v] < min_cost) {
                    min_cost = key[v];
                    u = v;
                }
            }
            // Exit if no node found
            if (u.empty()) {
                break;
            }
            // Set node in mst
            in_mst[u] = true;
            if (!parent[u].empty()) {
                string from = parent[u];
                const string& to = u;
                string edge_key = (from < to) ? from + to : to + from;
                edges[edge_key] = key[u];
            }
            for (const auto& edge: graph_edges.at(u)) {
                const string& v = edge.to;
                int cost = edge.cost;
                if (!in_mst[v] && cost < key[v]) {
                    key[v] = cost;
                    parent[v] = u;
                }
            }
        }
    }

    void kruskal_mst(const UndirectedGraph& ug) {
        struct FlatEdge {
            string from;
            string to;
            int cost;

            bool operator<(const FlatEdge& other) const {
                return cost < other.cost;
            }
        };

        edges.clear();
        auto edge_map = ug.get_edges();
        vector<FlatEdge> all_edges;
        for (const auto& [from, to, cost]: ug.get_unique_edges()) {
            all_edges.push_back({from, to, cost});
        }

        std::sort(all_edges.begin(), all_edges.end());

        Map<string, string> parent;
        for (const auto& [vertex, _]: ug.get_edges()) {
            parent[vertex] = vertex;
        }

        std::function<string(string)> find = [&](const string& x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };

        auto unite = [&](const string& a, const string& b) -> bool {
            string root_a = find(a);
            string root_b = find(b);
            if (root_a == root_b) {
                return false;
            }
            parent[root_a] = root_b;
            return true;
        };

        for (const auto& edge: all_edges) {
            if (unite(edge.from, edge.to)) {
                string key = (edge.from < edge.to) ? edge.from + edge.to : edge.to + edge.from;
                edges[key] = edge.cost;
            }
        }
    }

    void print() {
        int acc = 0;
        std::cout << "Minimal Spanning Tree" << std::endl << "Edge\tWeight" << std::endl;
        for (const auto& [key, value]: edges) {
            std::cout << key.substr(0, 3) << " - " << key.substr(3, 3) << "\t" << value << std::endl;
            acc += value;
        }
        std::cout << "Total Cost of MST: " << acc << std::endl;
    }
};

#endif
