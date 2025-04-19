
#ifndef AIRLINE_ROUTING_GRAPH_H
#define AIRLINE_ROUTING_GRAPH_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>
#include <tuple>

using std::string;
using std::exception;
using std::unordered_map;
using std::vector;
using std::tuple;

class Airport;

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

    bool is_terminal();
};

class Airport {
private:
    unordered_map<string, Flight*> edges;
    int incoming;
    int outgoing;
    string code; // maybe unnecessary
    string state;
public:
    Airport(string code, string state) : code(std::move(code)), state(std::move(state)), incoming(0), outgoing(0) {}

    [[nodiscard]] unordered_map<string, Flight*> get_edges() const {
        return edges;
    }

    [[nodiscard]] string get_code() const {
        return code;
    }

    void add_flight(const string& code_arrive, Airport* arrive, int distance, int cost) {
        edges.insert({code_arrive, new Flight(arrive, distance, cost)});
    }

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

bool Flight::is_terminal() {
    return destination->get_edges().empty();
}

class Graph {
private:
    unordered_map<string, vector<Airport*>> by_state;
    unordered_map<string, Airport*> vertexes;
public:
    Graph() = default;

    explicit Graph(size_t capacity) {
        vertexes = unordered_map<string, Airport*>(capacity);
    }

    [[nodiscard]] unordered_map<string, Airport*> get_vertexes() const {
        return vertexes;
    }

    [[nodiscard]] unordered_map<string, vector<Airport*>> get_states() const {
        return by_state;
    }

    [[nodiscard]] vector<string> get_all_airports() const {
        vector<string> v;
        for (const auto& vertex: vertexes) {
            v.push_back(vertex.first);
        }
        return v;
    }

    [[nodiscard]] int get_edge_dist(const string& from, const string& to) const {
        Airport* airport = vertexes.at(from);
        Flight* flight = airport->get_edges().at(to);
        return flight->get_distance();
    }

    [[nodiscard]] int get_edge_cost(const string& from, const string& to) const {
        Airport* airport = vertexes.at(from);
        Flight* flight = airport->get_edges().at(to);
        return flight->get_cost();
    }

    void add_airport(const string& code, const string& state) {
        auto ap = new Airport(code, state);
        vertexes.insert({code, ap});
        if (!by_state.contains(state)) {
            by_state.insert({state, {ap}});
        } else {
            by_state.at(state).push_back(ap);
        }
    }

    void add_flight(const string& code_depart, const string& code_arrive, int distance, int cost) {
        Airport* depart = vertexes.at(code_depart);
        Airport* arrive = vertexes.at(code_arrive);
        depart->inc_outgoing();
        arrive->inc_incoming();
        depart->add_flight(code_arrive, arrive, distance, cost);
    }

    bool airport_exists(const string& code) {
        return vertexes.contains(code);
    }

    struct MiniEdge {
        string code;
        int connections;

        MiniEdge(string code, int connections) : code(std::move(code)), connections(connections) {}


    };

    static bool compareMiniEdge(const MiniEdge& lhs, const MiniEdge& rhs) {
        return lhs.connections > rhs.connections;
    }

    void flight_connections() {
        vector<MiniEdge> v;
        std::cout << "Airport\tConnections" << std::endl;
        for (const auto& airport: vertexes) {
            v.emplace_back(airport.first, airport.second->total_flights());
        }
        std::sort(v.begin(), v.end(), compareMiniEdge);
        for (const MiniEdge& edge: v) {
            std::cout << edge.code << "\t" << edge.connections << std::endl;
        }
    }

    struct UndirectedEdge {
        int cost;
        string to;

    public:
        UndirectedEdge(int cost, string to) : cost(cost), to(std::move(to)) {}
    };

    class UndirectedGraph {
        unordered_map<string, vector<UndirectedEdge>> edges;

        int find_edge_index(const string& from, const string& to) const {
            const auto& neighbors = edges.at(from);
            for (size_t i = 0; i < neighbors.size(); i++) {
                if (neighbors[i].to == to) {
                    return static_cast<int>(i);
                }
            }
            return -1;
        }

    public:
        UndirectedGraph(const Graph& g) {
            for (const auto& vertex: g.get_all_airports()) {
                edges[vertex];
            }

            for (const auto& [vertex_key, vertex_value]: g.vertexes) {
                for (const auto& [key, value]: vertex_value->get_edges()) {
                    edges[vertex_key];
                    edges[key];
                    int idx_vertex = find_edge_index(vertex_key, key);
                    int idx_edge = find_edge_index(key, vertex_key);

                    if (idx_vertex != -1) {
                        if (value->get_cost() < edges[vertex_key][idx_vertex].cost) {
                            edges[vertex_key][idx_vertex].cost = value->get_cost();
                            edges[key][idx_edge].cost = value->get_cost();
                        }
                    } else {
                        edges[vertex_key].emplace_back(value->get_cost(), key);
                        edges[key].emplace_back(value->get_cost(), vertex_key);
                    }
                }
            }
        }

        [[nodiscard]] unordered_map<string, vector<UndirectedEdge>> get_edges() const {
            return edges;
        }

        vector<tuple<string, string, int>> get_unique_edges() const {
            vector<tuple<string, string, int>> result;
            vector<string> seen;
            for (const auto& [from, neighbors]: edges) {
                for (const auto& edge: neighbors) {
                    const string& to = edge.to;
                    string key = (from < to) ? from + to : to + from;
                    if (std::find(seen.begin(), seen.end(), key) == seen.end()) {
                        seen.push_back(key);
                        result.emplace_back(from, edge.to, edge.cost);
                    }
                }
            }
            return result;
        }
    };
};

#endif
