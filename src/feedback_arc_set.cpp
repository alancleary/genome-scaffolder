
#include "graphs.hpp"
#include "feedback_arc_set.hpp"

// use the macro for STL - enables the application to be run with the Standard Template Library or with other platforms
ILOSTLBEGIN

/* the ILP for the min FAS problem
 * T_in is the type of container for the ILP
 *      IloIntVarArray => ILP
 *      IloNumVarArray => relaxed ILP
 * T_out the type of results returned 
 *      int => ILP
 *      float => relaxed ILP */

typedef property_map<DirectedScaffoldGraph, edge_index_t>::type EdgeIndexMap;

// takes a directed graph and finds a min FAS for it
template<typename T_in,typename T_out >
T_out integer_linear_program( const DirectedScaffoldGraph &g, T_out* fas ) {
    // initialize fas
	int solution_size = -1,
		num_verts     = num_vertices( g ),
		num_arcs      = num_edges( g );
    for( int i = 0; i < num_arcs; i++ ) {
        fas[ i ] = 0;
    }
	EdgeIndexMap edge_index_map = get( edge_index, g );
    // create the environment object
    IloEnv env = IloEnv();

    try {
        // create the model object
        IloModel model = IloModel( env );
        // the feedback arc variables
        T_in b = T_in( env, num_arcs, 0, 1 );
        // the vertice variables
        IloIntVarArray ordering = IloIntVarArray( env, num_vertices, 1, num_vertices );
        // constrain the vertice numbering
		for( int i = 0; i < num_verts; i++ ) {
			model.add( 0 <= ordering[ i ] < num_arcs );
		}
        // constrain the vertice ordering
        directed_edge_itr ei, ei_end;
        for( tie( ei, ei_end ) = edges( g ); ei != ei_end; ++ei ) {
            model.add( ordering[ source( *ei, g ) ]-ordering[ target( *ei, g ) ] >= 1-num_vertices*b[ edge_index_map[ *ei ] ] );
        }
        // add the objective function
        model.add( IloMinimize( env, IloSum( b ) ) );
        // create the solver object
        IloCplex cplex = IloCplex( model );
        // disable output while solving
        cplex.setOut( env.getNullStream() );
        // solve the model
        cplex.solve();
        // get the solution
        IloNumArray vals(env);
        cplex.getValues(vals, b);
        //cplex.out() << "Solution status " << cplex.getStatus() << endl;
        //cplex.out() << "Objective value " << cplex.getObjValue() << endl;
        //cplex.out() << "Solution is: " << vals << endl;
        // populate the output array
        solution_size = 0;
        for( int k = 0; k < vals.getSize(); k++ ) {
            if( ( T_out )vals[ k ] ) {
                solution_size++;
            }
            fas[ k ] = ( T_out )vals[ k ];
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
