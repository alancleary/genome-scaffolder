#include "graphs.hpp"
#include "sign_enumeration.hpp"
#include "error_calculator.hpp"
#include <boost/graph/properties.hpp>
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <queue>

// stores results of cuthill-mckee
static std::vector<int> ordered_contigs;


// the binary tree
struct node {
    int contig; // value representing which contig
    int sign; // value representing orientation imposed on contig (-1 / 1)
    int p1; // cummulative sign violations
    int p2; // cummulative order violations
    int depth; //current depth in binary tree
    struct node *parent;
    struct node *l_child; // pointer to left child
    struct node *r_child; // pointer to right child
    node() { };
    //constructor for struct
    node( int contig, int sign, struct node *parent ) { 
        this->contig = contig;
        this->sign = sign;
        this->parent = parent;
        // implicit depth assignment
        if( parent != NULL ) {
            this->depth = parent->depth+1;
        } else {
            this->depth = 1;
        }
    }   

    // used by the priority queue
    bool operator< (const node& b) const{
        return ((p1 + p2) < (b.p1 + b.p2));
    };

    // returns the total error
    int error() {
        return p1 + p2;
    }
};


// priority queue to dictate exploration
static std::priority_queue<node> pq;

// saves the signs along a path into a given array
void assignment( int *signs, const node *n ) {
	signs[ n->contig ] = n->sign;
	if( n->parent ) {
		assignment( signs, n->parent );
	}
}
void save_assignment( int *signs, int num_verts, const node *n ) {
	for( int i = 0; i < num_verts; i++ ) {
		signs[ i ] = 0;
	}
	assignment( signs, n );
}

// recursive explorer function. Takes current node and corresponding index in cuthill-mckee ordering
void explore( node *parent, int &p, int *optimal_assignment, const ScaffoldGraph &g ) {
    // at internal node
    if( parent->depth < ordered_contigs.size() ) {
        // children of current node (originally root) should be vertex with next lower degree
        node l_child = node( ordered_contigs.at( parent->depth ), NEGATIVE, parent );
        node r_child = node( ordered_contigs.at( parent->depth ), POSITIVE, parent );
        // build tree
        parent->l_child = &l_child;
        parent->r_child = &r_child;
        // calculate each child's error using filtered graphs
		int num_verts = ordered_contigs.size();
		// compute the left child's error
		int *l_signs = new int[ num_verts ];
		save_assignment( l_signs, num_verts, &l_child );
		l_child.p1 = num_sign_violations( g, l_signs );
		int *l_fas = new int[ num_edges( g ) ];
		l_child.p2 = num_order_violations( g, l_signs, l_fas );
        //add to priority queue if error is less than current optimal
        if ( l_child.error() < p){
            pq.push(l_child);
        }
		// delete before recursing deeper
		delete [] l_signs;
		delete [] l_fas;
		// compute the right child's error
		int *r_signs = new int[ num_verts ];
		save_assignment( r_signs, num_verts, &r_child );
		r_child.p1 = num_sign_violations( g, r_signs );
		int *r_fas = new int[ num_edges( g ) ];
		r_child.p2 = num_order_violations( g, r_signs, r_fas );
        //add to priority queue if error is less than current optimal
        if ( r_child.error() < p){
            pq.push(r_child);
        }
		// delete before recursing deeper
		delete [] r_signs;
		delete [] r_fas;
        // use priority queue to determine next node to explore
        if (!pq.empty()){
            node new_parent = pq.top();
            pq.pop();
            explore(&new_parent, p, optimal_assignment, g);
        }
        else return;
    }
    // at leaf node
	else if( parent->error() < p ) {
        p = parent->error();
        assignment( optimal_assignment, parent );
    }
}

// main sign enumeration function
void sign_enumeration( int root, int *optimal_assignment, int &p, ScaffoldGraph &g ) {
    // reverse-cuthill mckee ordering
    cuthill_mckee_ordering( g, root, std::back_inserter( ordered_contigs ), get( &ScaffoldVertex::color, g ), get( &ScaffoldVertex::degree, g ) );
    // start exploring
    node n = node( root, POSITIVE, NULL );
    //n.depth = 1;
    explore( &n, p, optimal_assignment, g );
}
