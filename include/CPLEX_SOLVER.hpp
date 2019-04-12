//
// Created by tkhamvilai on 4/12/19.
//

#ifndef DIST_MILP_CPLEX_SOLVER_HPP
#define DIST_MILP_CPLEX_SOLVER_HPP

#include <iostream>
#include "MY_MACROS.hpp"

#ifdef CPLEX_AS_SOLVER
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>

class CPLEX_SOLVER{

    const char* LP_file;
    const char* Sol_file;

public:
    CPLEX_SOLVER(const char* LP_file, const char* Sol_file);

    int solve();

};
#endif

#endif //DIST_MILP_CPLEX_SOLVER_HPP
