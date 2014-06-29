
#include <stdlib.h>
#include <stdio.h>
#include "graphs.hpp"
#include "backbone.hpp"

void parse_command_line_args( int, char*[], int&, int&, int& );
void usage_quit( char* );

int main( int argc, char *argv[] ) {
    int num_verts,
        num_arcs,
        num_errors,
        max_error = 15;
    parse_command_line_args( argc, argv, num_verts, num_arcs, num_errors );

    // generate graph
    ScaffoldGraph g = generate_synthetic_graph( num_verts, num_arcs, num_errors );

    // edge bundling
	g = generate_bundled_graph( g );

    // backbone tree and start node
    int root = highest_degree( g );
    int signs[ num_verts ];
    backbone_sign_assignment( g, root, signs );

    // sign enurmoeration with binary tree

    // profit!
}

void parse_command_line_args( int argc, char *argv[], int &num_verts, int &num_arcs, int &num_errors ) {
    if( argc != 4 ) {
        usage_quit( argv[ 0 ] );
    }
    num_verts = std::stoi( argv[ 1 ] );
    num_arcs = std::stoi( argv[ 2 ] );
    num_arcs = std::stoi( argv[ 3 ] );
    if( num_verts <= 0 || num_arcs <= 0 || num_errors < 0 ) {
        puts( "Command line arguments must be positive integers" );
        usage_quit( argv[ 0 ] );
    } else if ( num_verts < 2 ) {
        puts( "At least two vertices required" );
        usage_quit( argv[ 0 ] );
    } else if ( num_arcs < num_verts-1 ) {
        puts( "Need enough edges for the graph to be simple" );
        usage_quit( argv[ 0 ] );
    } else if ( num_errors > num_verts+num_arcs ) {
        puts( "Too much error" );
        usage_quit( argv[ 0 ] );
    }
}

void usage_quit( char *program_name ) {
    printf( "Usage: %s num_vertices num_edges num_errors\n", program_name );
    exit( 1 );
}