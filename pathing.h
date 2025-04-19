
#ifndef AIRLINE_ROUTING_PATHING_H
#define AIRLINE_ROUTING_PATHING_H

#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "graph.h"
#include "util.h"

using std::string;
using std::vector;
using std::unordered_map;

const int INF = std::numeric_limits<int>::max();  // sentinel for "infinite" distance or cost

// Represents a single path with its sequence of airport codes, total distance, and total cost
struct Path {
    vector<string> path;    // ordered list of airport codes along this path
    int distance;           // accumulated distance of the path
    int cost;               // accumulated cost of the path

    Path() : distance(0), cost(0) {}            // default constructor

    explicit Path(const string& depart)          // initialize path starting at depart
            : distance(0), cost(0) {
        path = {depart};
    }

    void print_path() {                           // print this.path as A -> B -> C
        for (int i = 0; i < path.size() - 1; i++)
            std::cout << path[i] << " -> ";
        std::cout << path[path.size() - 1];
    }

    static void print_path(const vector<string>& path) {  // print arbitrary vector<string>
        for (int i = 0; i < path.size() - 1; i++)
            std::cout << path[i] << " -> ";
        std::cout << path[path.size() - 1];
    }
};

// Holds the results of a shortest-path search from a single origin
struct Paths {
    unordered_map<string, vector<Airport*>> by_state;  // map state code → list of airports in that state
    string from;                                       // origin airport code
    unordered_map<string, int> dist;                   // best-known distance from origin to each code
    unordered_map<string, string> prev;                // previous-hop map for path reconstruction
    unordered_map<string, int> cost;                   // best-known cost from origin to each code

    // construct with precomputed maps (moved in for efficiency)
    Paths(string from,
          unordered_map<string, vector<Airport*>> by_state,
          unordered_map<string, int> dist,
          unordered_map<string, int> cost,
          unordered_map<string, string> prev)
            : from(std::move(from)),
              by_state(std::move(by_state)),
              dist(std::move(dist)),
              cost(std::move(cost)),
              prev(std::move(prev)) {}

    // Print the single shortest path to the given airport code
    void to(const string& to) {
        Path p;
        p.distance = dist[to];                // retrieve distance
        p.cost = cost[to];                    // retrieve cost
        string code = to;
        p.path.push_back(code);               // start building reverse path
        while (contains(prev,code)) {         // walk back through prev[] until origin
            code = prev[code];
            p.path.push_back(code);
        }
        if (p.path.size() == 1) {             // no path found (only destination itself)
            std::cout << "Shortest route from " << from << " to " << to << ": None" << std::endl;
            return;
        }
        std::reverse(p.path.begin(), p.path.end());  // reverse to origin→destination
        std::cout << "Shortest route from " << from << " to " << to << ": ";
        p.print_path();
        std::cout << ". The length is " << p.distance << ". The cost is " << p.cost << "." << std::endl;
    }

    // Print all shortest paths from origin to airports in the given state code
    unordered_map<string, Path> to_state(const string& to) {
        unordered_map<string, Path> out;
        std::cout << "The shortest paths from " << from << " to " << to << " state airports are:" << std::endl;
        std::cout << std::endl << "Path\tLength\tCost" << std::endl;
        for (const Airport* airport: by_state[to]) {  // iterate airports in that state
            string code = airport->get_code();
            Path p;
            p.distance = dist[code];                // set distance
            p.cost = cost[code];                    // set cost
            p.path.push_back(code);                 // build reverse path
            while (contains(prev,code)) {
                code = prev[code];
                p.path.push_back(code);
            }
            if (p.path.size() == 1) continue;       // skip unreachable airports
            std::reverse(p.path.begin(), p.path.end());
            p.print_path();
            std::cout << "\t" << p.distance << "\t" << p.cost << std::endl;
        }
        return out;                                 // return map of code→Path if needed
    }
};

// Run Dijkstra-like shortest-path search from 'from' over Graph g
Paths find_paths_from(const Graph& g, const string& from) {
    unordered_map<string, int> dist;         // distance labels
    unordered_map<string, int> cost;         // cost labels
    unordered_map<string, bool> visited;     // visited flag per node
    unordered_map<string, string> prev;      // predecessor map

    // initialize all maps: origin=0, others=INF
    for (const auto& vertex: g.get_vertexes()) {
        dist.insert({vertex.first, vertex.first == from ? 0 : INF});
        cost.insert({vertex.first, vertex.first == from ? 0 : INF});
        visited.insert({vertex.first, false});
    }

    size_t unvisited = dist.size();          // count how many remain
    string current = from;                   // start at origin

    while (current != "") {                  // loop until no unvisited reachable node
        for (const auto& edge: g.get_vertexes()[current]->get_edges()) {
            if (!visited[edge.first] && (dist[current] + edge.second->get_distance()) < dist[edge.first]) {
                dist[edge.first] = dist[current] + edge.second->get_distance();  // relax by distance
                cost[edge.first] = cost[current] + edge.second->get_cost();      // relax by cost
                prev[edge.first] = current;                                // record predecessor
            }
        }
        visited[current] = true;             // mark done
        unvisited--;
        string u;
        int u_min = INF;
        // find next unvisited with smallest dist (no priority queue used)
        for (const auto& item: dist) {
            if (item.second < u_min && !g.get_vertexes()[item.first]->is_terminal() && !visited[item.first]) {
                u = item.first;
                u_min = item.second;
            }
        }
        current = u;                         // move to next node or "" if none remain
    }
    return {from, g.get_states(), dist, cost, prev};  // package results
}

// Run constrained shortest-path search with exactly 'stops' allowed
void find_path_with_n_stops(const Graph& g, const string& from, const string& to, int stops) {
    unordered_map<string, int> dist;      // distance labels
    unordered_map<string, int> cost;      // cost labels
    unordered_map<string, int> count;     // number of stops taken so far
    unordered_map<string, bool> visited;  // visited flag
    unordered_map<string, string> prev;   // predecessor map

    // initialize all maps: origin=0/0 stops, others=INF/0
    for (const auto& vertex: g.get_vertexes()) {
        dist.insert({vertex.first, vertex.first == from ? 0 : INF});
        cost.insert({vertex.first, vertex.first == from ? 0 : INF});
        count.insert({vertex.first, 0});
        visited.insert({vertex.first, false});
    }

    size_t unvisited = dist.size();
    string current = from;

    while (current != "") {
        for (const auto& edge: g.get_vertexes()[current]->get_edges()) {
            if (!visited[edge.first] && (dist[current] + edge.second->get_distance()) < dist[edge.first]) {
                if (edge.first != to || count[current] == stops) {  // enforce stop limit
                    dist[edge.first] = dist[current] + edge.second->get_distance();
                    cost[edge.first] = cost[current] + edge.second->get_cost();
                    count[edge.first] = count[current] + 1;
                    prev[edge.first] = current;
                }
            }
        }
        visited[current] = true;
        unvisited--;
        string u;
        int u_min = INF;
        // find next unvisited with smallest dist
        for (const auto& item: dist) {
            if (item.second < u_min && !g.get_vertexes()[item.first]->is_terminal() && !visited[item.first]) {
                u = item.first;
                u_min = item.second;
            }
        }
        current = u;
    }

    // reconstruct path for 'to'
    string code = to;
    vector<string> p;
    p.push_back(code);
    while (contains(prev, code)) {
        code = prev[code];
        p.push_back(code);
    }
    if (p.size() == 1) {  // no valid route found
        std::cout << "Shortest route from " << from << " to " << to << " with " << stops
                  << ((stops == 1) ? " stop: None" : " stops: None") << std::endl;
        return;
    }
    // print valid route
    std::cout << "The shortest route from " << from << " to " << to << " with " << stops
              << ((stops == 1) ? " stop: " : " stops: ");
    Path::print_path(p);
    std::cout << ". The length is " << dist[to] << ". The cost is " << cost[to] << "." << std::endl;
}

#endif  // AIRLINE_ROUTING_PATHING_H
