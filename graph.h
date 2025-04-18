
#ifndef AIRLINE_ROUTING_GRAPH_H
#define AIRLINE_ROUTING_GRAPH_H

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

using std::string;
using std::exception;
using std::unordered_map;
using std::vector;

class Airport;

class Flight {
private:
    Airport* destination;
    int distance;
    int cost;
public:
    Flight(Airport* destination, int distance, int cost): destination(destination), distance(distance), cost(cost) {}

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
    string code; // maybe unnecessary
    string state;
public:
    Airport(string  code, string  state): code(std::move(code)), state(std::move(state)) {}

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
            std::cout << vertex.first << std::endl;
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
        Airport* airport=vertexes.at(from);
        Flight* flight=airport->get_edges().at(to);
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
        depart->add_flight(code_arrive, arrive, distance, cost);
    }

    bool airport_exists(const string& code) {
        return vertexes.contains(code);
    }

};

#endif
