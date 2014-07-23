//
//  sign_enumeration.hpp
//  genome-scaffolder
//
//  Created by Adam Koziol on 7/10/14.
//  Copyright (c) 2014 MSU. All rights reserved.
//

#ifndef SIGN_ENUMERATION_H
#define SIGN_ENUMERATION_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
#include "graphs.hpp"

#define POSITIVE 1
#define NEGATIVE -1

struct node {
    int contig; //value representing which contig
    int sign;   //value representing sign assignment (-1 / 1)
	int error = -1;	//value representing current cumulative error
    struct node *left = NULL;  //pointer to left child
    struct node *right = NULL; //pointer to right child
    node(){};
    //constructor for struct
	node (int contig, int sign){
			this->contig = contig;
			this->sign = sign;
		}
};



void sign_enumeration(int, int[], int&, ScaffoldGraph&);
void explore(node*, int*, int&);

#endif /* SIGN_ENUMERATION_H */
