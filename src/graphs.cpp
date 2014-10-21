#include "graphs.hpp"
#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <boost/graph/graph_utility.hpp>

// boost random number generator
typedef mt19937 Random;
Random gen( time( 0 ) );
uniform_int<> dist( 0, std::numeric_limits<int>::max() );
variate_generator< Random, uniform_int<> > r( gen, dist );

// generates a scaffold graph with no errors (an acyclic graph)
InitialGraph generate_scaffold_graph( int num_verts, int num_arcs, int signs[] ) {
    InitialGraph g( num_verts );
    int s, t;
    edge_desc e;
    bool b;
    for( int i = 0; i < num_arcs; i++ ) {
		// make sure the graph is connected
		if( i < num_verts-1 ) {
			s = r()%(i+1);
			t = i+1;
		} else {
			// in an acyclic ordering the last node is never a source
			s = i%(num_verts-1);
			// the target needs to come after the source in the ordering
			t = r()%(num_verts-(s+1))+s+1;
		}
		// add the edge with acyclic ordering
		tie( e, b ) = add_edge( s, t, g );
		g[ e ].source.index = s;
		g[ e ].source.sign  = signs[ s ];
		g[ e ].target.index = t;
		g[ e ].target.sign  = signs[ t ];
		//g[ e ].label = g[ e ].source.sign*g[ e ].target.sign;
		// boost doesn't manage edge indexes for us
		//g[ e ].index = i;
    }
    return g;
}

// generates a list of signs to be assigned to a graph
void generate_signs( int *signs, int num_verts ) {
    for( int i = 0; i < num_verts; i++ ) {
        signs[ i ] = ( r()%2 ? POSITIVE : NEGATIVE );
    }
}

// imposes sign and order violations on a graph
void impose_error( InitialGraph &g, int num_errors ) {
    int num_verts = num_vertices( g );
    initial_edge_desc e;
    bool b;
	std::vector<initial_edge_desc> v;
    while( num_errors > 0 ) {
        tie( e, b ) = edge( r()%num_verts, r()%num_verts, g );
        if( b && std::find( v.begin(), v.end(), e) == v.end() ) {
            switch( r()%3 ) {
				// sign violations
				case 0:
					g[ e ].source.sign *= FLIP;
					//g[ e ].label	   *= FLIP;
					break;
				case 1:
					g[ e ].target.sign *= FLIP;
					//g[ e ].label       *= FLIP;
					break;
				// order violation
				case 2:
					int swap_index      = g[ e ].source.index;
					g[ e ].source.index = g[ e ].target.index;
					g[ e ].target.index = swap_index;
					int swap_sign		= g[ e ].source.sign;
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
InitialGraph generate_synthetic_graph( int num_verts, int num_arcs, int num_errors ) {
	int signs[ num_verts ];
	generate_signs( signs, num_verts );
    InitialGraph g = generate_scaffold_graph( num_verts, num_arcs, signs );
    impose_error( g, num_errors );
    return g;
}

// finds the highest degree vertice in the graph
int highest_degree( ScaffoldGraph &g ) {
    vertex_indices indices = get( vertex_index, g );
    int highest_degree = 0,
        highest_vertice,
        compare_degree;
    vertex_itr vi, vi_end;
    for( tie( vi, vi_end ) = vertices( g ); vi != vi_end; ++vi ) {
        compare_degree = out_degree( *vi, g );
        g[ *vi ].degree = -compare_degree;
        if( compare_degree > highest_degree ) {
            highest_degree = compare_degree;
            highest_vertice = indices[ *vi ];
        }
    }
    return highest_vertice;
}
/*
int highest_degree( ScaffoldGraph &g ) {
    int highest_degree = 0,
        highest_vertice,
        compare_degree;
    for( int i = 0; i < num_vertices( g ); i++ ) {
        compare_degree = out_degree( i, g );
		g[ i ].degree = -compare_degree;
        if( compare_degree > highest_degree ) {
            highest_degree  = compare_degree;
            highest_vertice = i;
        }
    }
    return highest_vertice;
}
*/

// bundle multiple edges between pairs of nodes
ScaffoldGraph generate_bundled_graph( const InitialGraph &g ) {
	int num_verts = num_vertices( g );
	ScaffoldGraph bundled( num_verts );
	// iterate over every pair of nodes
	initial_out_edge_itr ei, ei_end;
	int order_vote, sign_vote, index = 0;
    edge_desc e;
    bool b;
	for( int i = 0; i < num_verts-1; i++ ) {
		for( int j = i+1; j < num_verts; j++ ) {
			// iterate over all the edges between the nodes
			tie( ei, ei_end ) = edge_range( i, j, g );
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
                    bundled[ e ].target.index = i;
                    bundled[ e ].target.sign  = POSITIVE;
                    bundled[ e ].label        = bundled[ e ].source.sign;
                }
				//bundled[ e ].index = index++;
			}
		}
	}
    return bundled;
}

void print(ScaffoldGraph &g){
    print_graph(g);
    puts("");
}
