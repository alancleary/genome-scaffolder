
#include "graphs.hpp"

// generates a scaffold graph with no errors (an acyclic graph)
ScaffoldGraph generate_scaffold_graph( int num_verts, int num_arcs, int signs[] ) {
    ScaffoldGraph g( num_verts );
    int s, t;
    edge_desc e;
    bool b;
    for( int i = 0; i < num_arcs; i++ ) {
		// make sure the graph is connected
		if( i < num_verts ) {
			s = rand()%i;
			t = i;
		} else {
			// in an acyclic ordering the last node is never a source
			s = i%num_verts-1;
			// the target needs come after the source in the ordering
			t = rand()%(num_verts-s)+s+1;
		}
		// what's the orientation of the new edge?
		tie( e, b ) = add_edge( s, t, g );
		if( rand()%2 ) {
			g[ e ].source.index = s;
			g[ e ].source.sign  = signs[ s ];
			g[ e ].target.index = t;
			g[ e ].target.sign  = signs[ t ];
		} else {
			g[ e ].target.index = s;
			g[ e ].target.sign  = signs[ s ]*FLIP;
			g[ e ].source.index = t;
			g[ e ].source.sign  = signs[ t ]*FLIP;
		}
		g[ e ].label = g[ e ].source.sign*g[ e ].target.sign;
		// the index is used when constructing a backbone tree
		g[ e ].index = i;
    }
    return g;
}

// generates a list of signs to be assigned to a graph
void generate_signs( int *signs, int num_verts ) {
    for( int i = 0; i < num_verts; i++ ) {
        signs[ i ] = ( rand()%2 ? POSITIVE : NEGATIVE );
    }
}

// imposes sign and order violations on a graph
void impose_error( ScaffoldGraph &g, int num_errors ) {
    int s, t, num_verts = num_vertices( g );
    edge_desc e;
    bool b;
	std::vector<edge_desc> v;
    while( num_errors ) {
        tie( e, b ) = edge( rand()%num_verts, rand()%num_verts, g );
        if( b && std::find( v.begin(), v.end(), e) != v.end() ) {
            switch( rand()%3 ) {
				// sign violations
				case 0:
					g[ e ].source.sign *= FLIP;
					g[ e ].label *= FLIP;
					break;
				case 1:
					g[ e ].target.sign *= FLIP;
					g[ e ].label *= FLIP;
					break;
				// order violation
				case 2:
					int swap_index      = g[ e ].source.index;
					g[ e ].source.index = g[ e ].target.index;
					g[ e ].target.index = swap_index;
					int swap_sign    = g[ e ].source.sign;
					g[ e ].source.sign  = g[ e ].target.sign;
					g[ e ].target.sign  = swap_sign;
					break;
            }
			v.push_back( e );
            --num_errors;
        }
    }
}

// generates a scaffold graph with error
ScaffoldGraph generate_synthetic_graph( int num_verts, int num_arcs, int num_errors ) {
	int signs[ num_verts ];
	generate_signs( signs, num_verts );
    ScaffoldGraph g = generate_scaffold_graph( num_verts, num_arcs, signs );
    impose_error( g, num_errors );
    return g;
}

// finds the highest degree vertice in the graph
int highest_degree( const ScaffoldGraph &g ) {
    int highest_degree = 0,
        highest_vertice,
        compare_degree;
    for( int i = 0; i < num_vertices( g ); i++ ) {
        compare_degree = out_degree( i, g );
        if( compare_degree > highest_degree ) {
            highest_degree = compare_degree;
            highest_vertice = i;
        }
    }
    return highest_vertice;
}

// bundle multiple edges between pairs of nodes
ScaffoldGraph generate_bundled_graph( const ScaffoldGraph &g ) {
	int num_verts = num_vertices( g );
	ScaffoldGraph bundled( num_verts );
	// iterate over every pair of nodes
	out_edge_itr ei, ei_end;
	int order_vote, sign_vote;
    edge_desc e;
    bool b;
	for( int i = 0; i < num_verts-1; i++ ) {
		for( int j = i+1; j < num_verts; j++ ) {
			// iterate over all the edges between the nodes
			tie( ei, ei_end ) = edge_range( i, j, g );
			/*
			if( ei != ei_end ) {
				order_vote = sign_vote = 0;
				for( ei; ei != ei_end; ++ei ) {
					// i will be given a positive sign
					if( i == g[ *ei ].source.index ) {
                        if( g[ *ei ].source.sign == POSITIVE ) {
                            order_vote++;
                            sign_vote += g[ *ei ].target.sign;
                        } else {
                            order_vote--;
                            sign_vote -= g[ *ei ].target.sign;
                        }
					} else {
                        if( g[ *ei ].target.sign == POSITIVE ) {
                            order_vote--;
                            sign_vote += g[ *ei ].target.sign;
                        } else {
                            order_vote++;
                            sign_vote -= g[ *ei ].target.sign;
                        }
					}
				}
				// add the bundled edge to the new graph
                tie( e, b ) = add_edge( i, j, bundled );
                if( order_vote >= 0 ) {
                    bundled[ e ].source.index = i;
                    bundled[ e ].source.sign  = POSITIVE;
                    bundled[ e ].target.index = j;
                    bundled[ e ].target.sign  = ( ( sign_vote >= 0 ) ? POSITIVE : NEGATIVE );
                    bundled[ e ].label        = bundled[ e ].target.sign;
                } else {
                    bundled[ e ].source.index = j;
                    bundled[ e ].source.sign  = ( ( sign_vote >= 0 ) ? POSITIVE : NEGATIVE );
                    bundled[ e ].target.index = 1;
                    bundled[ e ].target.sign  = POSITIVE;
                    bundled[ e ].label        = bundled[ e ].source.sign;
                }
			}
		*/
		}
	}
    return bundled;
}
