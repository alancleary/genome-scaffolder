
#include "graphs.hpp"
#include "backbone.hpp"
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/functional/hash.hpp>
#include <sparsehash/dense_hash_map>

using google::dense_hash_map;      // namespace where class lives by default
using boost::hash;

// sets the weight of each edge to be the number of shared neighbors of the edge's vertices
// the edges of each shared vertex vote on the label of the edge
void assign_neighbor_weights_and_generate_labels( ScaffoldGraph &g, int *labels ) { 
    edge_index_map indices = get( edge_index, g );
    edge_itr ei, ei_end;
    out_edge_itr ej, ej_end;
    int s, n, t, vote;
	// iterate over all the edges in the graph
    for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ) {
        vote = 0;
		s = g[ *ei ].source.index;
		t = g[ *ei ].target.index;
		// make a hash to compare neighbors with
		dense_hash_map<const int, int, hash<const int> > neighbors;
		neighbors.set_empty_key(-1);
		// iterate all edges incident to the source
        for( tie( ej, ej_end ) = out_edges( s, g ); ej != ej_end; ++ej ) {
			// save each neighbor's label
			if( g[ *ej ].source.index == s ) {
				neighbors[ g[ *ej ].target.index ] = g[ *ej ].label;
			} else {
				neighbors[ g[ *ej ].source.index ] = g[ *ej ].label;
			}
		}
		// iterate all edges incident to the target
		for( tie( ej, ej_end ) = out_edges( t, g ); ej != ej_end; ++ej ) {
			if( g[ *ej ].source.index == t && neighbors[ g[ *ej ].target.index ] ) {
				vote += neighbors[ g[ *ej ].target.index ] * g[ *ej ].label;
			} else if( g[ *ej ].target.index == t && neighbors[ g[ *ej ].source.index ] ) {
				vote += neighbors[ g[ *ej ].source.index ] * g[ *ej ].label;
			}
		}
		// set the majority label based on the vote
        if( vote > 0 ) {
            //labels[ g[ *ei ].index ] = POSITIVE;
            labels[ indices[ *ei ] ] = POSITIVE;
        } else if( vote < 0 ) {
            //labels[ g[ *ei ].index ] = NEGATIVE;
            labels[ indices[ *ei ] ] = NEGATIVE;
        } else {
            //labels[ g[ *ei ].index ] = g[ *ei ].label;
            labels[ indices[ *ei ] ] = g[ *ei ].label;
        }
    }
}

// uses majority labels to generate a sign assignment for the graph
// how do we make edge_index_map play nice with const ScaffoldGraph?
void generate_sign_assignment( ScaffoldGraph &g, std::vector<edge_desc> mst_edges, int *labels, int root, int *signs ) {
    edge_index_map indices = get( edge_index, g );
    out_edge_itr ei, ei_end;
	// iterate edges incident to root
	for( tie( ei, ei_end ) = out_edges( root, g ); ei != ei_end; ++ei ) { 
		// only propagate signs along edges that are in the mst
		int i = std::find( mst_edges.begin(), mst_edges.end(), *ei ) - mst_edges.begin();
		if( i < mst_edges.size() ) {
			// propagate the sign
			int next,
				//sign = signs[ root ]*labels[ g[ *ei ].index ];
                sign = signs[ root ]*labels[ indices[ *ei ] ];
			if( root == g[ *ei ].source.index ) {
				next = g[ *ei ].target.index;
				signs[ g[ *ei ].target.index ] = sign;
			} else {
				next = g[ *ei ].source.index;
				signs[ g[ *ei ].source.index ] = sign;
			}
			// recursively call the function
			mst_edges.erase( mst_edges.begin()+i );
			generate_sign_assignment( g, mst_edges, labels, next, signs );
		}
	}
}

// prints the backbone sign assignment
void print_sign_assignment(ScaffoldGraph &g, int *signs){
	int length = num_vertices(g);
	puts("Printing backbone sign assignment...");
	for(int i = 0; i < length; i++){
		printf("contig: %d, sign: %d\n", i, signs[i]);
	}
	puts("");
}

void print_labels(ScaffoldGraph &g, int *labels, int size){
	edge_itr ei, ei_end;
	puts("Printing labels...");
	int start, end, label;
	for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ){
		start = g[ *ei ].source.index;
		end = g[ *ei ].target.index;
		label = g [ *ei ].label;
		printf("start: %d, end: %d, label: %d\n", start, end, label);
	}
	puts("");
}

// constructs spanning tree from majority weights and makes sign assignment with spanning tree majority labels
void backbone_sign_assignment( ScaffoldGraph &g, int root, int *signs ) {
	// assign neighbor weights and majority labels to the graph edges
	int labels_size = num_edges(g);
    int labels[ labels_size ];

    assign_neighbor_weights_and_generate_labels( g, labels );

    //print label assignments
    print_labels(g, labels, labels_size);

	// find a minimum spanning tree using the weights
    std::vector<edge_desc> mst_edges;
    //property_map<ScaffoldGraph, int ScaffoldEdge::*>::type weight_property_map = get(&ScaffoldEdge::weight, g);
    kruskal_minimum_spanning_tree( g, std::back_inserter( mst_edges ), weight_map( get(&ScaffoldEdge::weight, g) ) );
	// propagate signs along the edges using the majority labels
	signs[ root ] = POSITIVE;
    generate_sign_assignment( g, mst_edges, labels, root, signs );
}


