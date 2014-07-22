//
//  sign_enumeration.cpp
//  genome-scaffolder
//
//  Created by Adam Koziol on 7/10/14.
//  Copyright (c) 2014 MSU. All rights reserved.
//


#include "graphs.hpp"
#include "error_calculator.hpp"
#include "sign_enumeration.hpp"

//main sign enumeration function
void sign_enumeration(int root, int optimal_sign[], int &p, int num_verts){//, ScaffoldGraph &g){
    //create root node
    node root_node;
    root_node.sign = POSITIVE;
    root_node.contig = root;
    
    //reverse-cuthill mckee ordering
    std::vector<ScaffoldEdgeNode> inv_perm(num_verts);
    //cuthill_mckee_ordering(g, root_node, inv_perm.rbegin());
    //start exploring - make separate function?
    
    //calculate error
}

//recursive explorer function
void explore (node current_node){	//add cuthill-mckee ordering to parameters
	
}
