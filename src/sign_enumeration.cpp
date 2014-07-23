//
//  sign_enumeration.cpp
//  genome-scaffolder
//
//  Created by Adam Koziol on 7/10/14.
//  Copyright (c) 2014 MSU. All rights reserved.
//

#include "sign_enumeration.hpp"
#include "graphs.hpp"
#include "error_calculator.hpp"
#include <boost/graph/properties.hpp>
#include <boost/graph/cuthill_mckee_ordering.hpp>

//stores results of cuthill-mckee
static std::vector<int> ordered_contigs;

//main sign enumeration function
void sign_enumeration(int root, int optimal_sign [], int &p, ScaffoldGraph &g){
    //create root node
    node r = node(root, POSITIVE);
    node * root_node = &r;
    //reverse-cuthill mckee ordering
    //cuthill_mckee_ordering(g, ordered_contigs.rbegin());//, get(vertex_color, g), make_degree_map(g));
    cuthill_mckee_ordering(g, root, std::back_inserter( ordered_contigs ), get( &ScaffoldVertex::color, g ), get( &ScaffoldVertex::degree, g ) );
    puts("Reverse cuthill-mckee ordering starting at: " + root);
    for (int i = 0; i < ordered_contigs.size(); i++){
        printf("index: %d , contig: %d , degree: %d\n", i, ordered_contigs.at(i), g[ordered_contigs.at(i)].degree);
    }
    //start exploring
    int * depth = 0;
    explore(root_node, depth, p);
    //calculate error
}

//recursive explorer function.  Takes current node and corresponding index in cuthill-mckee ordering
void explore (node * current_node, int * depth, int &p){
    //calculate error for current node
    /*
        ERROR CALCULATION CODE HERE
    */

	//at internal node
    if (*depth < ordered_contigs.size() - 1){
        //children of current node (originally root) should be vertex with next lower degree
        node left_child = node(ordered_contigs.at(*depth + 1), NEGATIVE);
        node right_child = node(ordered_contigs.at(*depth + 1), POSITIVE);
        //build tree
        current_node->left = &left_child;
        current_node->right = &right_child;

        //compare error to determine which way to explore
        int left_error, right_error;
        /*
            ERROR CALCULATION CODE HERE
        */

        //explore left
        if(left_error < right_error && left_error <= p){
            *depth = *depth + 1;
            explore(current_node->left, depth, p);
        }
        //explore right
        else if (right_error < left_error && right_error <= p){
            *depth = *depth + 1;
            explore(current_node->right, depth, p);
        }
    }

    //at leaf node
    //compute total error for path through tree, update p if new optimal found
    
    //subtract 1 from depth counter and return
    *depth = *depth - 1;
    return;

}
