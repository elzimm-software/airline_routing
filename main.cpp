#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include "graph.h"
#include "pathing.h"

using std::string;
using std::ifstream;

string get_state_code(const string& city) {
    return city.substr(city.length() - 3, 2);
}

Graph parse_csv(const string& filename) {
    Graph g(64);
    ifstream file(filename);
    string line;
    string depart_code, arrive_code, depart_city, arrive_city, distance, cost;
    std::getline(file, line);
    while (file) {
        std::getline(file, line);
        std::istringstream ss(line);
        std::getline(ss, depart_code, ',');
        std::getline(ss, arrive_code, ',');
        std::getline(ss, depart_city, ',');
        std::getline(ss, depart_city, ',');
        std::getline(ss, arrive_city, ',');
        std::getline(ss, arrive_city, ',');
        std::getline(ss, distance, ',');
        std::getline(ss, cost, ',');
        if (!g.airport_exists(depart_code)) {
            g.add_airport(depart_code, get_state_code(depart_city));
        }
        if (!g.airport_exists(arrive_code)) {
            g.add_airport(arrive_code, get_state_code(arrive_city));
        }
        g.add_flight(depart_code,arrive_code,stoi(distance),stoi(cost));
    }
    return g;
}

int main() {
    Graph g = parse_csv("airports.csv");\
    /*
    auto p = find_paths_from(g,"IAD").to("MIA");
    p.print_path();
    std::cout << std::endl << p.distance << "  " << p.cost << std::endl;
    */

    auto p = find_paths_from(g,"ATL").to_state("FL");
    std::cout << "Path\tLength\tCost" << std::endl;
    for (auto& path: p) {
        path.second.print_path();
        std::cout << "\t" << path.second.distance<< "\t" << path.second.cost << std::endl;
    }
    return 0;
}