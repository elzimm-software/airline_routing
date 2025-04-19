#include <string>
#include <regex>
#include "graph.h"
#include "pathing.h"
#include "tree.h"

using std::string;
using std::ifstream;

int main() {
    Graph g = parse_csv("airports.csv");\
    //find_paths_from(g,"IAD").to("MIA");

    //find_paths_from(g,"ATL").to_state("FL");

    //find_path_with_n_stops(g, "LAX", "MIA", 3);

    //g.flight_connections();

    Tree prim, kruskal;

    prim.prim_mst(g);
    kruskal.kruskal_mst(g);

    kruskal.print();

    return 0;
}