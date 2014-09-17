#include "graphs.hpp"
#include <boost/graph/strong_components.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <typeinfo>
#include <ilcplex/ilocplex.h>

// filtered undirected scaffold graph
struct EdgePredicate {
	ScaffoldGraph local_g;
    const int *signs;
    EdgePredicate() { }
    EdgePredicate( const ScaffoldGraph g, const int *signs ) : local_g( g ) {
        this->signs = signs;
	}
    template <typename Edge>
    bool operator()( const Edge& e ) const {
		return signs[ local_g[ e ].source.index ] != 0 && signs[ local_g[ e ].target.index ] != 0;
	}
};

struct VertexPredicate {
    const int *signs;
    VertexPredicate() { }
    VertexPredicate( const int *signs ) {
        this->signs = signs;
	}
    template <typename Vertex>
    bool operator()( const Vertex& v ) const {
		return signs[ v ] != 0;
	}
};

typedef filtered_graph<ScaffoldGraph, EdgePredicate, VertexPredicate> FilteredScaffoldGraph;
typedef graph_traits<FilteredScaffoldGraph>::edge_iterator filtered_edge_itr;

// creates a filtered graph from a partial sign assignment
FilteredScaffoldGraph generate_filtered_graph( const ScaffoldGraph &g, const int *signs ) { 
    EdgePredicate edge_filter( g, signs );
	VertexPredicate vertex_filter( signs );
    FilteredScaffoldGraph fg( g, edge_filter, vertex_filter );
    return fg;
}

// compares a sign assignment to the edge labels of a graph
int num_sign_violations( const ScaffoldGraph &g, const int *signs ) {
	FilteredScaffoldGraph fg = generate_filtered_graph( g, signs );
	filtered_edge_itr ei, ei_end;
	int n = 0;
	for( tie( ei, ei_end ) = edges( fg ); ei != ei_end; ++ei ) {
		if( fg[ *ei ].label != signs[ fg[ *ei ].source.index ]*signs[ fg[ *ei ].target.index ] ) {
			n++;
		}
	}
	return n;
}

// a fast and effective heuristic for the feedback arc set problem (1993)
int approximate_fas( const DirectedScaffoldGraph &g, int* fas ) {
    // make a vector to "hold" the vertices of the graph
	std::vector<int> vertices;
    for( int i = 0; i < num_vertices( g ); i++ ) {
        vertices.push_back( i );
    }
    // a vector to hold sinks
	std::vector<int> sinks;
    // a vector to hold sources
	std::vector<int> sources;
    // a vector to hold removed sinks
    std::vector<int> removed_sinks;
    // a vector to hold removed sources
    std::vector<int> removed_sources;
    // populate the vertices, sinks, and sources vectors

    // run the heuristic
    while( !vertices.empty() ) {
        while( !sinks.empty() ) {
            //choose a sink (at the time of removal) u
            //removed_sinks.push_back( u )
            //G.remove( u )
        }
        while( !sources.empty() ) {
            //choose a source (at the time of removal) u
            //removed_sources.push_back( u )
            //G.remove( u )
        }
        if( !vertices.empty() ) {
            //choose a vertex u for which delta( u ) (outdegree - indegree) is a maximum
            //s1.append( u )// the paper contradicts itself
            //G.remove( u )
        }
    }
    //combine to make ordered list by concatinating sinks to sources
    //the sequence induces a FAS on the graph
    return -1;
}

// function prototype
template<typename T_in,typename T_out>
T_out integer_linear_program( const DirectedScaffoldGraph&, T_out* );

// uses a sign assignment to construct a directed graph and solves the FAS
int num_order_violations( const ScaffoldGraph &g, const int *signs, int *fas ) {
	FilteredScaffoldGraph fg = generate_filtered_graph( g, signs );
	// initialize the fas array
	for( int i = 0; i < num_edges( fg ); i++ ) {
		fas[ i ] = 0;
	}
	// make the directed graph
	int num_verts = num_vertices( fg );
	DirectedScaffoldGraph dg( num_verts );
	// add edges to the directed graph
	filtered_edge_itr ei, ei_end;
	int s, s_i, t, t_i;
	for( tie( ei, ei_end ) = edges( fg ); ei != ei_end; ++ei ) {
		s   = fg[ *ei ].source.sign;
		s_i = fg[ *ei ].source.index;
		t   = fg[ *ei ].target.sign;
		t_i = fg[ *ei ].target.index;
		// if both signs match keep the ordering
		if( s == signs[ s_i ] && t == signs[ t_i ] ) {
			add_edge( s_i, t_i, dg );
		// if both signs are opposite reverse the ordering
		} else if( s != signs[ s_i ] && t != signs[ t_i ] ) {
			add_edge( t_i, s_i, dg );
		// if one is flipped keep the ordering from the original edge
		} else {
			add_edge( s_i, t_i, dg );
		}
	}
	// find (Tarjan's) strongly connected components
	std::vector<int> component_assignments( num_verts );
	int num_components = strong_components( dg, make_iterator_property_map( component_assignments.begin(), get( vertex_index, dg ), component_assignments[ 0 ] ) );
	// group the vertices by their components
	std::vector<int> components[ num_components ];
    std::vector<int>::iterator vi;
    for( vi = component_assignments.begin(); vi != component_assignments.end(); ++vi ) {
        components[ *vi ].push_back( vi - component_assignments.begin() );
    }
	// make a subgraph from each component and find a FAS for it
	int j, n = 0;
	directed_edge_itr dei, dei_end;
	for( int i = 0; i < num_components; i++ ) {
		// only components with three or more vertices will have a FAS
		if( components[ i ].size() > 2 ) {
			DirectedScaffoldGraph& sdg = dg.create_subgraph( components[ i ].begin(), components[ i ].end() );
			int sfas[ num_edges( fg ) ];
			n += integer_linear_program<IloIntVarArray, int>( sdg, sfas );
            //n += approximate_fas( sdg, sfas );
			// add sfas to fas
			j = 0;
			for( tie( dei, dei_end ) = edges( sdg ); dei != dei_end; ++dei ) {
				fas[ get( edge_index, sdg, *dei ) ] = sfas[ j++ ];
			}
		}
	}
	return n;
}

// feedback arc set solvers

// use the macro for STL - enables the application to be run with the Standard Template Library or with other platforms
ILOSTLBEGIN

/* the ILP for the min FAS problem
 * T_in is the type of container for the ILP
 *      IloIntVarArray => ILP
 *      IloNumVarArray => relaxed ILP
 * T_out the type of results returned 
 *      int => ILP
 *      float => relaxed ILP */

// takes a directed graph and finds a min FAS for it
template<typename T_in,typename T_out>
T_out integer_linear_program( const DirectedScaffoldGraph &g, T_out* fas ) {
    // initialize fas
    int solution_size = 10000000,
        num_verts     = num_vertices( g ),
        num_arcs      = num_edges( g );
    for( int i = 0; i < num_arcs; i++ ) {
        fas[ i ] = 0;
    }
    // create the environment object
    IloEnv env = IloEnv();

    try {
        // create the model object
        IloModel model = IloModel( env );
        // the feedback arc variables
        T_in b = T_in( env, num_arcs, 0, 1 );
        // the vertice variables
        IloIntVarArray ordering = IloIntVarArray( env, num_verts, 1, num_verts );
        // constrain the vertice numbering
        for( int i = 0; i < num_verts; i++ ) {
            model.add( 0 <= ordering[ i ] < num_arcs );
        }
        // constrain the vertice ordering
        directed_edge_itr ei, ei_end;
		int i = 0;
        for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ) {
            model.add( ordering[ source( *ei, g ) ]-ordering[ target( *ei, g ) ] >= 1-num_verts*b[ i++ ] );
        }
        // add the objective function
        model.add( IloMinimize( env, IloSum( b ) ) );
        // create the solver object
        IloCplex cplex = IloCplex( model );
        // disable output while solving
        cplex.setOut( env.getNullStream() );
        // solve the model
        cplex.solve();
		// print the results
        //IloNumArray vals(env);
        //cplex.getValues(vals, b); 
        //cplex.out() << "Solution status " << cplex.getStatus() << endl;
        //cplex.out() << "Objective value " << cplex.getObjValue() << endl;
        //cplex.out() << "Solution is: " << vals << endl;
        // populate the output array
        solution_size = 0;
        for( int k = 0; k < b.getSize(); k++ ) {
			fas[ k ] = ( T_out )cplex.getValue( b[ k ] );
			if( fas[ k ] ) {
                solution_size++;
            }
        }

        //return ( T_out )cplex.getObjValue();
    } catch( IloException& e ) {
        //cerr << "Concert exception caught: " << e << endl;
    } catch( ... ) {
        //cerr << "Unknown exception caught: " << endl;
    }
    // free the memory that was being used
    env.end();

    return solution_size;
}

