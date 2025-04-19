
#ifndef AIRLINE_ROUTING_TREE_H
#define AIRLINE_ROUTING_TREE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <limits>
#include <functional>
#include "graph.h"

using std::vector;
using std::string;
using std::unordered_map;
using UndirectedGraph = Graph::UndirectedGraph;

// Minimal Spanning Tree
class Tree {
    unordered_map<string, int> edges;

public:
    // Prim's MST generation algorithm
    // Essentially the same as Dijkstra just a different end structure
    void prim_mst(const UndirectedGraph& ug) {
        edges.clear();
        const auto& graph_edges = ug.get_edges();
        if (graph_edges.empty()) return;
        unordered_map<string, bool> in_mst;
        unordered_map<string, int> key;
        unordered_map<string, string> parent;

        vector<string> vertices;
        for (const auto& [vertex, _]: graph_edges) {
            vertices.push_back(vertex);
            key[vertex] = std::numeric_limits<int>::max();
            in_mst[vertex] = false;
        }

        string start = vertices[0];
        key[start] = 0;

        for (size_t count = 0; count < vertices.size(); count++) {
            string u;
            int min_cost = std::numeric_limits<int>::max();
            for (const auto& v: vertices) {
                if (!in_mst[v] && key[v] < min_cost) {
                    min_cost = key[v];
                    u = v;
                }
            }
            if (u.empty()) {
                break;
            }
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

    // Kruskal MST generation algorithm
    // Connect the smallest edges possible, avoiding cycles, until MST is complete
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

        // Sort edges by weight
        std::sort(all_edges.begin(), all_edges.end());

        unordered_map<string, string> parent;
        // Set node as its own parent
        for (const auto& [vertex, _]: ug.get_edges()) {
            parent[vertex] = vertex;
        }

        // Recursive find implementation
        std::function<string(string)> find = [&](const string& x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        };

        // Prevents cycles from forming
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

    // Print either format of tree
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
