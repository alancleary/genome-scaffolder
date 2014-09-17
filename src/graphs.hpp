#ifndef GRAPHS_HPP
#define GRAPHS_HPP

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>

using namespace boost;

#define FLIP -1
#define POSITIVE 1
#define NEGATIVE -1
#define AGREE 1
#define DISAGREE -1

// paired-end data
struct PairedEnd {
    int index;
    int sign;
};

// initial graph
struct InitialEdge {
    PairedEnd source;
    PairedEnd target;
};

typedef adjacency_list<multisetS, vecS, undirectedS, no_property, InitialEdge> InitialGraph;
typedef graph_traits<InitialGraph>::edge_descriptor initial_edge_desc;
typedef graph_traits<InitialGraph>::out_edge_iterator initial_out_edge_itr;

// undirected scaffold graph
struct ScaffoldVertex {
	int degree;
	default_color_type color;
};

struct ScaffoldEdge {
    PairedEnd source;
	PairedEnd target;
	int label;
    int weight;
};

typedef adjacency_list<vecS, vecS, undirectedS, ScaffoldVertex, property<edge_index_t, int, ScaffoldEdge> > ScaffoldGraph;
typedef graph_traits<ScaffoldGraph>::edge_descriptor edge_desc;
typedef graph_traits<ScaffoldGraph>::edge_iterator edge_itr;
typedef graph_traits<ScaffoldGraph>::out_edge_iterator out_edge_itr;
typedef graph_traits<ScaffoldGraph>::vertex_iterator vertex_itr;
typedef property_map<ScaffoldGraph, vertex_index_t>::type vertex_indices;
typedef property_map<ScaffoldGraph, edge_index_t>::type edge_index_map;

// subundirected graph
typedef subgraph<ScaffoldGraph> SubScaffoldGraph;

// directed scaffold graph
typedef subgraph< adjacency_list<vecS, vecS, directedS, property<vertex_color_t, int>, property<edge_index_t, int> > > DirectedScaffoldGraph;
typedef graph_traits<DirectedScaffoldGraph>::edge_iterator directed_edge_itr;

// graph functions
InitialGraph generate_synthetic_graph( int, int, int );
int highest_degree( ScaffoldGraph& );
ScaffoldGraph generate_bundled_graph( const InitialGraph& );

#endif /* GRAPHS_HPP */
