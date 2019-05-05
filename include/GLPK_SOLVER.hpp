//
// Created by tkhamvilai on 4/12/19.
//

#ifndef DIST_MILP_GLPK_SOLVER_HPP
#define DIST_MILP_GLPK_SOLVER_HPP

#include <iostream>
#include "MY_MACROS.hpp"

#include <glpk.h>
#include "NOC_GLPK.hpp"

class GLPK_SOLVER{

    const char* LP_file;
    const char* Sol_file;

public:
    GLPK_SOLVER(const char* LP_file, const char* Sol_file);

    int solve();
    int solve(NOC_GLPK *NoC_GLPK);

};

#endif //DIST_MILP_GLPK_SOLVER_HPP
