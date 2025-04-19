
#ifndef AIRLINE_ROUTING_TREE_H
#define AIRLINE_ROUTING_TREE_H

#include <unordered_map>
#include <string>
#include <vector>
#include "graph.h"

using std::vector;
using std::string;
using std::unordered_map;
using UndirectedGraph = Graph::UndirectedGraph;

class Tree {
     unordered_map<string, int> edges;

public:
     void prim_mst(const UndirectedGraph& ug) {
         for (const auto& [key, values]: ug.get_edges()) {
             for (const auto& e: values) {
                 string combine;
                 if (key > e.to) {
                     combine = key+e.to;
                 } else {
                     combine = e.to+key;
                 }
                 edges.insert({combine, e.cost});
             }
         }

     }

     void print() {
         int acc = 0;
         std::cout << "Minimal Spanning Tree" << std::endl << "Edge\tWeight"<<std::endl;
         for (const auto& [key, value]: edges) {
             std::cout << key.substr(0,3) << " - " << key.substr(3, 3) << "\t"<< value <<std::endl;
             acc+=value;
         }
         std::cout << "Total Cost of MST: " << acc << std::endl;
     }
};

#endif
