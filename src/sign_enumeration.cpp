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
void sign_enumeration(int root, int optimal_sign [], int p, ScaffoldGraph &g){
    //create root node
    node * root_node = new node(root, POSITIVE);
    printf("root node passed to sign enumeration: %d\n", root);
    //reverse-cuthill mckee ordering
    //cuthill_mckee_ordering(g, ordered_contigs.rbegin());//, get(vertex_color, g), make_degree_map(g));
    cuthill_mckee_ordering(g, root, std::back_inserter( ordered_contigs ), get( &ScaffoldVertex::color, g ), get( &ScaffoldVertex::degree, g ) );
    puts("Reverse cuthill-mckee ordering starting at: " + root);
    for (int i = 0; i < ordered_contigs.size(); i++){
        printf("index: %d , contig: %d , degree: %d\n", i, ordered_contigs.at(i), g[ordered_contigs.at(i)].degree);
    }
    puts("\nBackbone sign assignment: ");
    for (int i = 0; i < ordered_contigs.size(); i++){
        printf("index: %d , sign: %d\n", i, optimal_sign[i]);
    }
    //start exploring
    int depth = 0;
    //pointer to leaf node of current optimal sign assignment path through tree
    node * optimal_leaf = NULL;
    printf("Starting exploration of binary tree at node %d\n", ordered_contigs.at(0));
    printf("Ordered contigs array size: %d\n", ordered_contigs.size());
    explore(root_node, &depth, &p, optimal_leaf);

    //find optimal sign assignment
    printf("Reading binary tree for optimal sign assignment.\n");
    find_optimal(root_node, optimal_leaf, optimal_sign);
}

//recursive explorer function.  Takes current node and corresponding index in cuthill-mckee ordering
void explore (node * current_node, int * depth, int *p, node * optimal_leaf){
    printf("Depth: %d\n", *depth);
    printf("At node: %d\n", current_node->contig);
    printf("P value: %d\n", *p);
	//at internal node
    if (*depth < ordered_contigs.size() - 1){
        //children of current node (originally root) should be vertex with next lower degree
        node *left_child = new node(ordered_contigs.at(*depth + 1), NEGATIVE);
        node *right_child = new node(ordered_contigs.at(*depth + 1), POSITIVE);
    
        //assign random error (for testing)
        // left_child->error = rand() % 5;
        // right_child->error = rand() % 5;

        //build tree

        current_node->left = left_child;
        current_node->right = right_child;

        //compare error (sign + order) to determine which way to explore
        int left_error = 0; 
        int right_error = 0;
        /*
            ERROR CALCULATION CODE HERE
        */

        //add current node error
        left_error = left_error + current_node->error;
        right_error = right_error + current_node->error;

        left_child->error = left_error;
        right_child->error = right_error;

        printf("Assigned errors: %d %d\n", left_child->error, right_child->error);


        //explore left then right
        if(left_child->error <= right_child->error && left_child->error <= *p){
            printf("Going left\n");
            *depth = *depth + 1;
            explore(current_node->left, depth, p, optimal_leaf);
            if (right_child->error <= *p){
                printf("Going right\n");
                *depth = *depth + 1;
                explore(current_node->right, depth, p, optimal_leaf);
            }
        }
        //explore right then left
        else if (right_child->error <= left_child->error && right_child->error <= *p){
            printf("Going right\n");
            *depth = *depth + 1;
            explore(current_node->right, depth, p, optimal_leaf);
            if (left_child->error <= *p){
                printf("Going left\n");
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
    if (current_node->error < *p){
        puts("Found better optimal value. Reassigning.");
        *p = current_node->error;
        optimal_leaf = current_node;
    }
    //subtract 1 from depth counter and return
    *depth = *depth - 1;
    puts("Reached leaf.  Going up.");
    return;

}

/*
    Once the binary sign assignment tree has been explored, and an optimal sign
    enumeration found, this function writes the optimal assignment to the optimal sign array,
    which is passed back to the main function.
*/
bool find_optimal(node * current_node, node * optimal_leaf, int optimal_sign []){
    if (current_node == NULL){
        return false;
    }

    if (current_node == optimal_leaf || find_optimal(current_node->left, optimal_leaf, optimal_sign)
        || find_optimal(current_node->right, optimal_leaf, optimal_sign)){

        optimal_sign[current_node->contig] = current_node->sign;
        return true;
    }
    return false;
}
