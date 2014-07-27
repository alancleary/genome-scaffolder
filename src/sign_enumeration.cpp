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
    r.error = 0;
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
    //pointer to leaf node of current optimal sign assignment path through tree
    node * optimal_leaf = NULL;
    explore(root_node, depth, p, optimal_leaf);
    //calculate error
}

//recursive explorer function.  Takes current node and corresponding index in cuthill-mckee ordering
void explore (node * current_node, int * depth, int &p, node * optimal_leaf){
	//at internal node
    if (*depth < ordered_contigs.size() - 1){
        //children of current node (originally root) should be vertex with next lower degree
        node left_child = node(ordered_contigs.at(*depth + 1), NEGATIVE);
        node right_child = node(ordered_contigs.at(*depth + 1), POSITIVE);
        //build tree
        current_node->left = &left_child;
        current_node->right = &right_child;

        //compare error (sign + order) to determine which way to explore
        int left_error, right_error;
        /*
            ERROR CALCULATION CODE HERE
        */
        //add current node error
        left_error = left_error + current_node->error;
        right_error = right_error + current_node->error;

        left_child.error = left_error;
        right_child.error = right_error;

        //explore left then right
        if(left_error < right_error && left_error <= p){
            *depth = *depth + 1;
            explore(current_node->left, depth, p, optimal_leaf);
            if (right_error <= p){
                *depth = *depth + 1;
                explore(current_node->right, depth, p, optimal_leaf);
            }
        }
        //explore right then left
        else if (right_error < left_error && right_error <= p){
            *depth = *depth + 1;
            explore(current_node->right, depth, p, optimal_leaf);
            if (left_error <= p){
                *depth = *depth + 1;
                explore(current_node->left, depth, p, optimal_leaf);
            }
        }
    }

    //at leaf node
    /*
        compute total error for path through tree, update p if new optimal found
        set optimal_leaf pointer to leaf node of optimal sign assignment
    */
    if (current_node->error < p){
        p = current_node->error;
        optimal_leaf = current_node;
    }
    //subtract 1 from depth counter and return
    *depth = *depth - 1;
    return;

}
