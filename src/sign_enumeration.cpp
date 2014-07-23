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
    //start exploring
    explore(root_node, 0);
    //calculate error
}

//recursive explorer function.  Takes current node and corresponding index in cuthill-mckee ordering
void explore (node * current_node, int i){
	//children of current node (originally root) should be vertex with next lower degree
    if (i < ordered_contigs.size()){
        node left_child = node(ordered_contigs.at(i+1), NEGATIVE);
        node right_child = node(ordered_contigs.at(i+1), POSITIVE);
        //build tree
        current_node->left = &left_child;
        current_node->right = &right_child;
    }

    //compare error to determine which way to explore
    int left_error, right_error;

    //go left
    if (left_error > right_error){
        explore(current_node->left, i+1);
    }
    //go right
    else{
        explore(current_node->right, i+1);
    }
}
