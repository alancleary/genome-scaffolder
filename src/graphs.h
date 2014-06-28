
#define POSITIVE 1
#define NEGATIVE -1

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

struct ScaffoldEdge {
    int s;
    int s_sign;
    int t;
    int t_sign;
    int weight;
    int index;
};

typedef adjacency_list<vecS, vecS, undirectedS, no_property, ScaffoldEdge> ScaffoldGraph;
typedef graph_traits<ScaffoldGraph>::edge_descriptor edge_desc;
typedef graph_traits<ScaffoldGraph>::edge_iterator edge_itr;
typedef graph_traits<ScaffoldGraph>::out_edge_iterator out_edge_itr;

//property_map<ScaffoldGraph, int ScaffoldEdge::*>::type weight_property_map = get(&ScaffoldEdge::weight, g);

ScaffoldGraph generate_simple_synthetic_graph( int, int, int );
int highest_degree( const ScaffoldGraph& );

