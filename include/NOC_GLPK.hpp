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

    NOC_GLPK();

    void write_LP(NOC *NoC);
    void CreateModel(NOC *NoC);
    void read_Sol(NOC *NoC);
    void DeleteModel(NOC *NoC);
};
#endif

#endif //DIST_MILP_NOC_GLPK_HPP
