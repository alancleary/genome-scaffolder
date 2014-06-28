
#include "graphs.h"
#include "backbone.h"
#include <boost/graph/kruskal_min_spanning_tree.hpp>

// sets the weight of each edge to be the number of shared neighbors of the edge's vertices
// the edges of each shared vertex vote on the label of the edge
void assign_neighbor_weights_and_generate_labels( ScaffoldGraph &g, int *labels ) { 
    edge_itr ei, ei_end;
    out_edge_itr s_ei, s_ei_end,
                 t_ei, t_ei_out;
    int s, n, t, vote;
	// iterate over all the edges in the graph
    for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ) { 
        vote = 0;
		s = g[ *ei ].source.index;
		t = g[ *ei ].target.index;
		// iterate all edges incident to the source
        for( tie( s_ei, s_ei_end ) = out_edges( s, g ); s_ei != s_ei_end; ++s_ei ) { 
			// get the neighbor of the source
            if( g[ *s_ei ].source.index == s ) {
                n = g[ *s_ei ].target.index;
            } else {
                n = g[ *s_ei ].source.index;
            }
			// iterate all edges incident to the target
            for( tie( t_ei, t_ei_out ) = out_edges( t, g ); t_ei != t_ei_out; ++t_ei ) { 
				// if the edge isn't that of s and t and one of the edges is n
                if( n != t && ( n == g[ *t_ei ].source.index || n == g[ *t_ei ].target.index ) ) { 
					// add to the neighbor weight of the edge of s and t
                    g[ *ei ].weight += 1;
					// vote for equality
					vote += g[ *s_ei ].label*g[ *t_ei ].label;
                }
            }
            if( vote > 0 ) {
                labels[ g[ *ei ].index ] = POSITIVE;
            } else if( vote < 0 ) {
                labels[ g[ *ei ].index ] = NEGATIVE;
            } else {
                labels[ g[ *ei ].index ] = g[ *ei ].label;
            }
        }
    }
}

// creates majority labels for the edges in the mst and uses them to generate a sign assignment
void generate_signs( const ScaffoldGraph &g, std::vector<edge_desc> mst_edges, int *labels, int root, int *signs ) {
}

// constructs spanning tree from majority weights and makes sign assignment with spanning tree majority labels
void backbone_sign_assignment( ScaffoldGraph &g, int root, int *signs ) {
	// assign neighbor weights and majority labels to the graph edges
    int labels[ num_edges( g ) ];
    assign_neighbor_weights_and_generate_labels( g, labels );
	// find a minimum spanning tree using the weights
    std::vector<edge_desc> mst_edges;
    property_map<ScaffoldGraph, int ScaffoldEdge::*>::type weight_property_map = get(&ScaffoldEdge::weight, g);
    kruskal_minimum_spanning_tree( g, std::back_inserter( mst_edges ), weight_map( weight_property_map ) );
	// propagate signs along the edges using the majority labels
    generate_signs( g, mst_edges, labels, root, signs );
}
