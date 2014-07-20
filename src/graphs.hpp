#ifndef GRAPHS_H
#define GRAPHS_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include <vector>
#include <boost/graph/properties.hpp>

using namespace boost;

#define FLIP -1
#define POSITIVE 1
#define NEGATIVE -1
#define AGREE 1
#define DISAGREE -1

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

typedef adjacency_list<multisetS, vecS, undirectedS, no_property, ScaffoldEdge> ScaffoldGraph;
typedef graph_traits<ScaffoldGraph>::edge_descriptor edge_desc;
typedef graph_traits<ScaffoldGraph>::edge_iterator edge_itr;
typedef graph_traits<ScaffoldGraph>::out_edge_iterator out_edge_itr;
typedef graph_traits<ScaffoldGraph>::vertex_descriptor Vertex;

typedef subgraph< adjacency_list<vecS, vecS, directedS, property<vertex_color_t, int>, property<edge_index_t, int> > > DirectedScaffoldGraph;
typedef graph_traits<DirectedScaffoldGraph>::edge_iterator directed_edge_itr;

ScaffoldGraph generate_synthetic_graph( int, int, int );
int highest_degree( const ScaffoldGraph& );
ScaffoldGraph generate_bundled_graph( const ScaffoldGraph& );

#endif /* GRAPHS_H */
