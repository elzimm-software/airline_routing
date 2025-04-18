
#ifndef AIRLINE_ROUTING_GRAPH_H
#define AIRLINE_ROUTING_GRAPH_H

#include <string>
#include <iostream>
#include "map.h"

using std::string;
using std::exception;



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
};

class Airport {
private:
    HashMap<Flight*> edges;
    string code; // maybe unnecessary
    string state;
public:
    Airport(string  code, string  state): code(std::move(code)), state(std::move(state)) {}

    [[nodiscard]] HashMap<Flight*> get_edges() const {
        return edges;
    }

    void add_flight(const string& code_arrive, Airport* arrive, int distance, int cost) {
        edges.insert(code_arrive, new Flight(arrive, distance, cost));
    }

};

class Graph {
private:
    HashMap<vector<Airport*>> by_state;
    HashMap<Airport*> vertexes;
public:
    Graph() = default;

    explicit Graph(size_t capacity) {
        vertexes = HashMap<Airport*>(capacity);
    }

    [[nodiscard]] HashMap<Airport*> get_vertexes() const {
        return vertexes;
    }

    void add_airport(const string& code, const string& state) {
        auto ap = new Airport(code, state);
        vertexes.insert(code, ap);
        if (!by_state.contains(state)) {
            by_state.insert(state, {ap});
        } else {
            by_state.get(state).push_back(ap);
        }
    }

    void add_flight(const string& code_depart, const string& code_arrive, int distance, int cost) {
        Airport* depart;
        if (!vertexes.at(code_depart, depart)) {
            throw std::runtime_error("Invalid departure airport");
        }
        Airport* arrive;
        if (!vertexes.at(code_arrive, arrive)) {
            throw std::runtime_error("Invalid arrival airport");
        }
        depart->add_flight(code_arrive, arrive, distance, cost);
    }

    bool airport_exists(const string& code) {
        return vertexes.contains(code);
    }

};

#endif
