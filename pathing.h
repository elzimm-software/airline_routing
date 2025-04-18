#ifndef AIRLINE_ROUTING_PATHING_H
#define AIRLINE_ROUTING_PATHING_H

#include <limits>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "graph.h"

using std::string;
using std::vector;
using std::unordered_map;

const int INF = std::numeric_limits<int>::max();

struct Path {
    vector<string> path;
    int distance;
    int cost;

    Path(): distance(0), cost(0) {};

    explicit Path(const string& depart):distance(0),cost(0) {
        path = {depart};
    }

    void print_path() {
        for (int i = 0; i < path.size()-1; i++) {
            std::cout << path[i]<< " -> ";
        }
        std::cout << path[path.size()-1];
    }
};

struct Paths {
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    unordered_map<string, int> cost;

    Paths(unordered_map<string, int> dist, unordered_map<string, int> cost, unordered_map<string, string> prev): dist(std::move(dist)), cost(std::move(cost)), prev(std::move(prev)) {}

    Path to(const string& to) {
        Path p;
        p.distance = dist[to];
        p.cost = cost[to];
        string code = to;
        p.path.push_back(code);
        while (prev.contains(code)) {
            code = prev[code];
            p.path.push_back(code);
        }
        std::reverse(p.path.begin(),p.path.end());
        return p;
    }
};

Paths find_paths_from(const Graph& g, const string& from) {
    unordered_map<string,int> dist;
    unordered_map<string,int> cost;
    unordered_map<string, bool> visited;
    unordered_map<string,string> prev;

    for (const auto& [key,value]: g.get_vertexes()) {
            dist.insert({key, key == from ? 0 : INF});
            cost.insert({key, key == from ? 0 : INF});
            visited.insert({key, false});
    }

    size_t unvisited = dist.size();
    string current = from;

    while (current != "") {
        for (const auto& [key, value]: g.get_vertexes()[current]->get_edges()) {
            if (!visited[key] && (dist[current] + value->get_distance()) < dist[key]) {
                dist[key] = dist[current] + value->get_distance();
                cost[key] = cost[current] + value->get_cost();
                prev[key] = current;
            }
        }
        visited[current] = true;
        unvisited--;
        string u;
        int u_min = INF;
        for (const auto& [key, value]: dist) {
            if (value < u_min && !g.get_vertexes()[key]->is_terminal() && !visited[key]) {
                u = key;
                u_min = value;
            }
        }
        current = u;
    }
    return {dist,cost,prev};
}

#endif
