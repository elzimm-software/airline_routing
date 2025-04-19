#include <string>
#include "graph.h"
#include "pathing.h"
#include "tree.h"

using std::string;
using std::ifstream;

int main() {
    auto g = Graph("airports.csv"); // Task 1
    find_paths_from(g,"IAD").to("MIA"); // Task 2
    find_paths_from(g,"ATL").to_state("FL"); // Task 3
    find_path_with_n_stops(g, "LAX", "MIA", 3); // Task 4
    g.flight_connections(); // Task 5

    // Task 6 not represented here
    // conversion from Graph to UndirectedGraph performed implicitly
    // when passing Graph to a function that takes UndirectedGraph

    Tree prim, kruskal;
    prim.prim_mst(g); // Task 7
    kruskal.kruskal_mst(g); // Task 8
    prim.print();
    kruskal.print();

    return 0;
}