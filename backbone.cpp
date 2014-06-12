
#include "graphs.h"

// sets the weight of each edge to be the number of shared neighbors of the edge's vertices
// the edges of each shared vertex vote on the label of the edge
void assign_neighbor_weights_and_generate_labels( ScaffoldGraph &g, int *labels ) { 
    edge_itr ei, ei_end,
             s_ei, s_ei_end,
             t_ei, t_ei_out;
    int s, s_i, t, vote;
    for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ) { 
        vote = 0;
        s = source( *ei, g );
        t = target( *ei, g );
        for( tie( s_ei, s_ei_end ) = out_edges( s, g ); s_ei != s_ei_end; ++s_ei ) { 
            if( source( *s_ei, g ) == s ) {
                s_i = target( *s_ei, g );
            } else {
                s_i = source( *s_ei, g );
            }
            for( tie( t_ei, t_ei_end ) = out_edges( t, g ); t_ei != t_ei_end; ++t_ei ) { 
                if( s_i != t && ( s_i == source( *t_ei, g ) || s_i == target( *t_ei, g ) ) ) { 
                    g[ *ei ].weight += 1;
                    if( g[ *s_ei ].s_sign*g[ *s_ei ].t_sign == g[ *t_ei ].s_sign*g[ *t_ei ].t_sign ) {
                        vote++;
                    } else {
                        vote--;
                    }
                }
            }
            if( vote > 0 ) {
                labels[ g[ *ei ].index ] = POSITIVE;
            } else if( vote < 0 ) {
                labels[ g[ *ei ].index ] = NEGATIVE;
            } else {
                labels[ g[ *ei ].index ] = g[ *ei ].s_sign*g[ *ei ].t_sign;
            }
        }
    }
}

// creates majority labels for the edges in the mst and uses them to generate a sign assignment
void generate_signs( const ScaffoldGraph &g, std::vector<edge_desc> mst_edges, int *labels, int root, int *signs ) {
}

// constructs spanning tree from majority weights makes signs assignment with spanning tree majority labels
void backbone_sign_assignment( ScaffoldGraph &g, int root, int *signs ) {
    int labels[ num_edges( g ) ];
    assign_neighbor_weights( g, labels );
    std::vector<edge_desc> mst_edges;
    kruskal_minimum_spanning_tree( g, std::back_inserter( mst ), weight_map( weight_property_map ) );
    generate_signs( g, mst_edges, labels, root, signs );
}
