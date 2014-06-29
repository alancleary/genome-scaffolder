#ifndef FEEDBACK_ARC_SET_H
#define FEEDBACK_ARC_SET_H

#include <ilcplex/ilocplex.h>

template<typename T_in,typename T_out>
T_out integer_linear_program( const DirectedScaffoldGraph&, T_out* );

#endif /* FEEDBACK_ARC_SET_H */
