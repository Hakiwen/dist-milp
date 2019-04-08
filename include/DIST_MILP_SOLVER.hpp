//
// Created by tkhamvilai on 4/5/19.
//

#ifndef DIST_MILP_DIST_MILP_SOLVER_HPP
#define DIST_MILP_DIST_MILP_SOLVER_HPP

#include <iostream>

#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>

class DIST_MILP_SOLVER{

    const char* LP_file;
    const char* Sol_file;

public:
    DIST_MILP_SOLVER(const char* LP_file, const char* Sol_file);

    int solve();
};

#endif //DIST_MILP_DIST_MILP_SOLVER_HPP
