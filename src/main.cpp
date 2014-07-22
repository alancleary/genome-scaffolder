
#include <stdlib.h>
#include <stdio.h>
#include "graphs.hpp"
#include "backbone.hpp"
#include "error_calculator.hpp"
#include "sign_enumeration.hpp"

void parse_command_line_args( int, char*[], int&, int&, int& );
void usage_quit( char* );

int main( int argc, char *argv[] ) {
	// parse command line arguments
    int num_verts,
        num_arcs,
        num_errors,
        max_error = 15;
    parse_command_line_args( argc, argv, num_verts, num_arcs, num_errors );

    // generate graph
	puts("generating scaffold graph");
    ScaffoldGraph g = generate_synthetic_graph( num_verts, num_arcs, num_errors );

    // edge bundling
	puts("performing edge bundling");
	g = generate_bundled_graph( g );

    // backbone tree and start node
	puts("getting highest degree");
    int root = highest_degree( g );
	printf("%d\n", root);
    int signs[ num_verts ];
	puts("getting the backbone sign assignment");
    backbone_sign_assignment( g, root, signs );
	int fas[ num_arcs ];
	puts("calculating sign and order violations");
	int p1 = num_sign_violations( g, signs ),
		p2 = num_order_violations( g, signs, fas ),
		p = p1+p2;
	printf("sign violations: %d\n", p1);
	printf("order violations: %d\n", p2);
	printf("total violations: %d\n", p);

    // sign enumeration
    //global array to store current optimal sign assignment - initially the backbone
    int optimal_sign[num_verts];
    std::copy(signs, signs + num_verts, optimal_sign);
    sign_enumeration(root, optimal_sign, p, num_verts);//, g);
    // profit!
	puts("profit!");
}

void parse_command_line_args( int argc, char *argv[], int &num_verts, int &num_arcs, int &num_errors ) {
    if( argc != 4 ) {
        usage_quit( argv[ 0 ] );
    }
    num_verts = std::stoi( argv[ 1 ] );
    num_arcs = std::stoi( argv[ 2 ] );
    num_errors = std::stoi( argv[ 3 ] );
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
