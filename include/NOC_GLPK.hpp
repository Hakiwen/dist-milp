//
// Created by tkhamvilai on 4/12/19.
//

#ifndef DIST_MILP_NOC_GLPK_HPP
#define DIST_MILP_NOC_GLPK_HPP

#include "MY_MACROS.hpp"

#include <iostream>
#include <Eigen/Dense>

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

#endif //DIST_MILP_NOC_GLPK_HPP
