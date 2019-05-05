//
// Created by tkhamvilai on 4/12/19.
//

#ifndef DIST_MILP_NOC_GLPK_HPP
#define DIST_MILP_NOC_GLPK_HPP

#include <iostream>
#include <Eigen/Dense>
#include "MY_MACROS.hpp"

#ifdef GLPK_AS_SOLVER

#include <glpk.h>
#include "NOC.hpp"

class NOC_GLPK {

public:
    glp_prob *model;
    const char* LP_file; // problem file name
    const char* Sol_file;

    NOC_GLPK(const char* LP_file, const char* Sol_file);

    void write_LP(NOC *NoC);
    void CreateModel(NOC *NoC);
    void read_Sol(NOC *NoC);
    void DeleteModel(NOC *NoC);
};
#endif

#endif //DIST_MILP_NOC_GLPK_HPP
