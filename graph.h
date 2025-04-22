
#ifndef AIRLINE_ROUTING_GRAPH_H
#define AIRLINE_ROUTING_GRAPH_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <tuple>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "util.h"

using std::string;
using std::exception;
using std::unordered_map;
using std::vector;
using std::tuple;
using std::ifstream;
using std::istringstream;

// Allows reference as pointer in flight before being fully declared
class Airport;

// Directional edges of graph
// Stores destination pointer, distance weight, and cost weight.
class Flight {
private:
    Airport* destination;
    int distance;
    int cost;
public:
    Flight(Airport* destination, int distance, int cost) : destination(destination), distance(distance), cost(cost) {}

    [[nodiscard]] int get_distance() const {
        return distance;
    }

    [[nodiscard]] int get_cost() const {
        return cost;
    }

    // Implementation below to allow use in Airport
    bool is_terminal();
};

// Vertex of graph
// Stores map of edges, number of incoming and outgoing flights, IATA code, and state abbreviation
class Airport {
private:
    unordered_map<string, Flight*> edges; // IATA code is key, pointer to Flight is value
    int incoming;
    int outgoing;
    string code;
    string state;
public:
    Airport(string code, string state) : code(std::move(code)), state(std::move(state)), incoming(0), outgoing(0) {}

    [[nodiscard]] unordered_map<string, Flight*> get_edges() const {
        return edges;
    }

    [[nodiscard]] string get_code() const {
        return code;
    }

    // Inserts a new flight into the graph
    // Assumes destination code is a valid vertex
    void add_flight(const string& code_arrive, Airport* arrive, int distance, int cost) {
        // Insert will not overwrite duplicate keys
        edges.insert({code_arrive, new Flight(arrive, distance, cost)});
    }

    // If airport has no departing flights
    bool is_terminal() {
        return edges.empty();
    }

    void inc_incoming() {
        incoming++;
    }

    void inc_outgoing() {
        outgoing++;
    }

    int total_flights() {
        return incoming + outgoing;
    }
};

// If the destination of this flight has no departing flights
bool Flight::is_terminal() {
    return destination->get_edges().empty();
}

// Extracts state code from the "city, state" string
string get_state_code(const string& city) {
    return city.substr(city.length() - 3, 2);
}

// Primary data structure for the application
// Stores a map of all airports and all airports in each state.
class Graph {
private:
    unordered_map<string, vector<Airport*>> by_state; // State abbreviation is key, vector of pointers to airports in said state is value
    unordered_map<string, Airport*> vertexes; // IATA code is key, pointer to airport is value
public:
    Graph() = default;

    // Parses a graph from given csv or csv formated plaintext
    // File extension is irrelevant as long as formatting is correct
    explicit Graph(const string& filename) {
        // Initialize empty map of vertexes
        vertexes = unordered_map<string, Airport*>();
        // Open file
        ifstream file(filename);
        string line;
        string depart_code, arrive_code, depart_city, arrive_city, distance, cost;
        // Ignore first line
        std::getline(file, line);
        // Iterate through line of file reading data from each
        while (file) {
            std::getline(file, line);
            // Convert line to stringstream to allow comma delimited parsing
            istringstream ss(line);
            std::getline(ss, depart_code, ',');
            std::getline(ss, arrive_code, ',');
            // Read depart and arrive city twice to ignore comma in "city, state"
            std::getline(ss, depart_city, ',');
            std::getline(ss, depart_city, ',');
            std::getline(ss, arrive_city, ',');
            std::getline(ss, arrive_city, ',');
            std::getline(ss, distance, ',');
            std::getline(ss, cost, ',');
            // If airport is new, add it to vertexes
            if (!airport_exists(depart_code)) {
                add_airport(depart_code, get_state_code(depart_city));
            }
            if (!airport_exists(arrive_code)) {
                add_airport(arrive_code, get_state_code(arrive_city));
            }
            // Add flight to specified airport
            add_flight(depart_code, arrive_code, stoi(distance), stoi(cost));
        }
    }

    [[nodiscard]] unordered_map<string, Airport*> get_vertexes() const {
        return vertexes;
    }

    [[nodiscard]] unordered_map<string, vector<Airport*>> get_states() const {
        return by_state;
    }

    // Returns a vector of all IATA codes
    [[nodiscard]] vector<string> get_all_airports() const {
        vector<string> v;
        for (const auto& vertex: vertexes) {
            v.push_back(vertex.first);
        }
        return v;
    }

    // Returns distance for an edge given the edge's departure and arrival codes
    [[nodiscard]] int get_edge_dist(const string& from, const string& to) const {
        Airport* airport = vertexes.at(from);
        Flight* flight = airport->get_edges().at(to);
        return flight->get_distance();
    }

    // Returns cost for an edge given the edge's departure and arrival codes
    [[nodiscard]] int get_edge_cost(const string& from, const string& to) const {
        Airport* airport = vertexes.at(from);
        Flight* flight = airport->get_edges().at(to);
        return flight->get_cost();
    }

    // Adds a new airport
    void add_airport(const string& code, const string& state) {
        // Dynamically allocate airport
        auto ap = new Airport(code, state);
        // Insert will not overwrite duplicate airports
        vertexes.insert({code, ap});
        // If state is new create it
        if (!contains(by_state,state)) {
            by_state.insert({state, {ap}});
        } else { // Else add airport to state vector
            by_state.at(state).push_back(ap);
        }
    }

    // Add a new flight
    void add_flight(const string& code_depart, const string& code_arrive, int distance, int cost) {
        // Get departure and arrival airport
        Airport* depart = vertexes.at(code_depart);
        Airport* arrive = vertexes.at(code_arrive);
        // Increment appropriate counters for both airports
        depart->inc_outgoing();
        arrive->inc_incoming();
        // Add flight to airport
        depart->add_flight(code_arrive, arrive, distance, cost);
    }

    bool airport_exists(const string& code) {
        return contains(vertexes, code);
    }

    // Simplified edge struct for use in flight_connections function
    struct MiniEdge {
        string code;
        int connections;

        MiniEdge(string code, int connections) : code(std::move(code)), connections(connections) {}
    };

    // Comparison function for std::sort
    static bool compareMiniEdge(const MiniEdge& lhs, const MiniEdge& rhs) {
        return lhs.connections > rhs.connections;
    }

    // Counts the number of flights in and out of each airport and displays them in descending order
    void flight_connections() {
        vector<MiniEdge> v;
        std::cout << "Airport\tConnections" << std::endl;
        // Place each code, num connections pair in a vector
        for (const auto& airport: vertexes) {
            v.emplace_back(airport.first, airport.second->total_flights());
        }
        // Sort the vector descending
        std::sort(v.begin(), v.end(), compareMiniEdge);
        // Print each pair in order
        for (const MiniEdge& edge: v) {
            std::cout << edge.code << "\t" << edge.connections << std::endl;
        }
    }

    // Edge with no direction
    struct UndirectedEdge {
        int cost;
        string to;

    public:
        UndirectedEdge(int cost, string to) : cost(cost), to(std::move(to)) {}
    };

    // Graph with nondirection edges
    // Stores a map of IATA codes and vectors of edges
    class UndirectedGraph {
        unordered_map<string, vector<UndirectedEdge>> edges;

        // Helper function to locate an edge between two vertexes
        // Returns -1 if no edge exists
        int find_edge_index(const string& from, const string& to) const {
            const auto& neighbors = edges.at(from);
            // Iterate over all edges in one vertex and return the one whose destination matches the other vertex
            for (size_t i = 0; i < neighbors.size(); i++) {
                if (neighbors[i].to == to) {
                    // Cast i from size_t to int for convenience
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

    public:
        // Constructor allows implicit conversion from Graph to UndirectedGraph
        // Allowing Graph objects to be passed directly to UndirectedGraph functions
        UndirectedGraph(const Graph& g) {
            // Create all vertexes
            for (const auto& vertex: g.get_all_airports()) {
                edges[vertex];
            }

            // Iterate over vertexes
            for (const auto& vertex: g.vertexes) {
                // Iterate over edges of each vertex
                for (const auto& edge: vertex.second->get_edges()) {
                    // Ensure vertexes exist
                    // This it seems like it does the same as edges[vertex] above, but it doesnt work right without it
                    // So we just don't touch it
                    edges[vertex.first];
                    edges[edge.first];
                    // locate the indexes the edge in both directions
                    int idx_vertex = find_edge_index(vertex.first, edge.first);
                    int idx_edge = find_edge_index(edge.first, vertex.first);

                    // Verify index exists
                    if (idx_vertex != -1) {
                        // Only update vertex if cost is less than existing cost
                        if (edge.second->get_cost() < edges[vertex.first][idx_vertex].cost) {
                            edges[vertex.first][idx_vertex].cost = edge.second->get_cost();
                            edges[edge.first][idx_edge].cost = edge.second->get_cost();
                        }
                    } else { // Else create vertex
                        edges[vertex.first].emplace_back(edge.second->get_cost(), edge.first);
                        edges[edge.first].emplace_back(edge.second->get_cost(), vertex.first);
                    }
                }
            }
        }

        [[nodiscard]] unordered_map<string, vector<UndirectedEdge>> get_edges() const {
            return edges;
        }

        // Returns a tuple of unique edges, ignoring duplicates
        vector<tuple<string, string, int>> get_unique_edges() const {
            vector<tuple<string, string, int>> result;
            vector<string> seen;
            // Iterate over vertexes
            for (const auto& vertex: edges) {
                // Iterate over each edge
                for (const auto& edge: vertex.second) {
                    // Convert edge from tuple half to string
                    const string& to = edge.to;
                    // Concatenate string in alphabetical order
                    string key = (vertex.first < to) ? vertex.first + to : to + vertex.first;
                    // If key has not been seen
                    if (std::find(seen.begin(), seen.end(), key) == seen.end()) {
                        // See it
                        seen.push_back(key);
                        // Place into output vector
                        result.emplace_back(vertex.first, edge.to, edge.cost);
                    }
                }
            }
            return result;
        }
    };
};

#endif
