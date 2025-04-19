#ifndef AIRLINE_ROUTING_PATHING_H
#define AIRLINE_ROUTING_PATHING_H

#include <limits>
#include <string>
#include <utility>
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

    Path() : distance(0), cost(0) {};

    explicit Path(const string& depart) : distance(0), cost(0) {
        path = {depart};
    }

    void print_path() {
        for (int i = 0; i < path.size() - 1; i++) {
            std::cout << path[i] << " -> ";
        }
        std::cout << path[path.size() - 1];
    }

    static void print_path(const vector<string>& path) {
        for (int i = 0; i < path.size() - 1; i++) {
            std::cout << path[i] << " -> ";
        }
        std::cout << path[path.size() - 1];
    }
};

struct Paths {
    unordered_map<string, vector<Airport*>> by_state;
    string from;
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    unordered_map<string, int> cost;

    Paths(string from, unordered_map<string, vector<Airport*>> by_state, unordered_map<string, int> dist,
          unordered_map<string, int> cost, unordered_map<string, string> prev) : from(std::move(from)),
                                                                                 by_state(std::move(by_state)),
                                                                                 dist(std::move(dist)),
                                                                                 cost(std::move(cost)),
                                                                                 prev(std::move(prev)) {}

    void to(const string& to) {
        Path p;
        p.distance = dist[to];
        p.cost = cost[to];
        string code = to;
        p.path.push_back(code);
        while (prev.contains(code)) {
            code = prev[code];
            p.path.push_back(code);
        }
        if (p.path.size() == 1) {
            std::cout << "Shortest route from " << from << " to " << to << ": None" << std::endl;
            return;
        }
        std::reverse(p.path.begin(), p.path.end());
        std::cout << "Shortest route from " << from << " to " << to << ": ";
        p.print_path();
        std::cout << ". The length is " << p.distance << ". The cost is " << p.cost << "." << std::endl;
    }

    unordered_map<string, Path> to_state(const string& to) {
        unordered_map<string, Path> out;
        std::cout << "The shortest paths from " << from << " to " << to << " state airports are:" << std::endl;
        std::cout << std::endl << "Path\tLength\tCost" << std::endl;
        for (const Airport* airport: by_state[to]) {
            string code = airport->get_code();
            Path p;
            p.distance = dist[code];
            p.cost = cost[code];
            p.path.push_back(code);
            while (prev.contains(code)) {
                code = prev[code];
                p.path.push_back(code);
            }
            if (p.path.size() == 1) {
                continue;
            }
            std::reverse(p.path.begin(), p.path.end());
            p.print_path();
            std::cout << "\t" << p.distance << "\t" << p.cost << std::endl;
        }
        return out;
    }
};

Paths find_paths_from(const Graph& g, const string& from) {
    unordered_map<string, int> dist;
    unordered_map<string, int> cost;
    unordered_map<string, bool> visited;
    unordered_map<string, string> prev;

    for (const auto& [key, value]: g.get_vertexes()) {
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
    return {from, g.get_states(), dist, cost, prev};
}

void find_path_with_n_stops(const Graph& g, const string& from, const string& to, int stops) {
    unordered_map<string, int> dist;
    unordered_map<string, int> cost;
    unordered_map<string, int> count;
    unordered_map<string, bool> visited;
    unordered_map<string, string> prev;

    for (const auto& [key, value]: g.get_vertexes()) {
        dist.insert({key, key == from ? 0 : INF});
        cost.insert({key, key == from ? 0 : INF});
        count.insert({key, 0});
        visited.insert({key, false});
    }

    size_t unvisited = dist.size();
    string current = from;

    while (current != "") {
        for (const auto& [key, value]: g.get_vertexes()[current]->get_edges()) {
            if (!visited[key] && (dist[current] + value->get_distance()) < dist[key]) {
                if (key != to || count[current] == stops) {
                    dist[key] = dist[current] + value->get_distance();
                    cost[key] = cost[current] + value->get_cost();
                    count[key] = count[current] + 1;
                    prev[key] = current;
                }
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
    string code = to;
    vector<string> p;
    p.push_back(code);
    while (prev.contains(code)) {
        code = prev[code];
        p.push_back(code);
    }
    if (p.size() == 1) {
        std::cout << "Shortest route from " << from << " to " << to << " with " << stops
                  << ((stops == 1) ? " stop: None" : " stops: None") << std::endl;
        return;
    }
    std::cout << "The shortest route from " << from << " to " << to << " with " << stops
              << ((stops == 1) ? " stop: " : " stops: ");
    Path::print_path(p);
    std::cout << ". The length is " << dist[to] << ". The cost is " << cost[to] << "." << std::endl;
}

#endif
