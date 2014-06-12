
#include "graphs.h"

// generates a scaffold graph with no errors, except for cycles...
ScaffoldGraph generate_simple_scaffold_graph( int num_verts, int num_arcs, int signs[] ) {
    std::vector<int> connected;
    connected.push_back( num_verts-1 );
    ScaffoldGraph g( num_verts );
    int s, t;
    edge_desc e, bool b;
    for( int i = 0; i < num_arcs; i++ ) {
        s = i%num_vertices;
        if( i < num_verts ) { 
            t = rand()%connected.size();
            tie( e, b ) = add_edge( s, connected[ t ], g );
            connected.push_back( s );
        } else {
            b = false;
            while( !b ) {
                t = rand()%num_verts;
                if( t != s && !edge( s, t, g ).second ) {
                    tie( e, b ) = add_edge( s, t, g );
                }
            }
        }
        g[ e ].s = s;
        g[ e ].s_sign = signs[ s ];
        g[ e ].t = t;
        g[ e ].t_sign = signs[ t ];
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

// imposes error on a graph
void impose_error( ScaffoldGraph &g, int num_errors ) {
    int type, s, t,
        num_verts = num_vertices( g );
    edge_desc e, e_new;
    bool b;
    while( num_errors ) {
        tie( e, b ) = edge( rand()%num_verts, rand()%num_verts, g );
        if( b ) {
            type = rand()%3;
            if( type == 2 ) {
                g[ e ].s_sign *= NEGATIVE;
            } else if ( type == 1 ) {
                g[ e ].t_sign *= NEGATIVE;
            } else {
                int temp = g[ e ].s;
                g[ e ].s = g[ e ].t;
                g[ e ].t = temp;
                temp = g[ e ].s_sign;
                g[ e ].s_sign = g[ e ].t_sign;
                g[ e ].t_sign = temp;
            }
            --num_errors;
        }
    }
}

// generates a simple scaffold graph with error
ScaffoldGraph generate_simple_synthetic_graph( int num_verts, int num_arcs, int num_errors ) {
    int signs[ num_verts ];
    generate_signs( signs );
    ScaffoldGraph g = generate_simple_scaffold_graph( num_verts, num_arcs, signs );
    impose_error( g );
    return g;
}

// finds the highest degree vertice in the graph
int highest_degree( const Graph &g ) {
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

