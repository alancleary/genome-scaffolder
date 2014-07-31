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

// undirected scaffold graph
struct ScaffoldVertex {
	int degree;
	default_color_type color;
};

struct ScaffoldEdgeNode {
	int index;
	int sign;
};

struct ScaffoldEdge {
    ScaffoldEdgeNode source;
	ScaffoldEdgeNode target;
	int label;
    int weight;
    int index;
};

typedef adjacency_list<multisetS, vecS, undirectedS, ScaffoldVertex, ScaffoldEdge> ScaffoldGraph;
typedef graph_traits<ScaffoldGraph>::edge_descriptor edge_desc;
typedef graph_traits<ScaffoldGraph>::edge_iterator edge_itr;
typedef graph_traits<ScaffoldGraph>::out_edge_iterator out_edge_itr;

// directed scaffold graph
typedef subgraph< adjacency_list<vecS, vecS, directedS, property<vertex_color_t, int>, property<edge_index_t, int> > > DirectedScaffoldGraph;
typedef graph_traits<DirectedScaffoldGraph>::edge_iterator directed_edge_itr;

// graph functions
ScaffoldGraph generate_synthetic_graph( int, int, int );
int highest_degree( ScaffoldGraph& );
ScaffoldGraph generate_bundled_graph( const ScaffoldGraph& );

#endif /* GRAPHS_HPP */
