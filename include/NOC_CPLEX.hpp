//
// Created by tkhamvilai on 4/4/19.
//

#ifndef DIST_MILP_NOC_CPLEX_HPP
#define DIST_MILP_NOC_CPLEX_HPP

#include <iostream>

#include <Eigen/Dense>

#ifdef __x86_64__
#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>
#endif

#include "pugixml.hpp"

#include "NOC.hpp"

class NOC_CPLEX {
    typedef IloArray<IloNumVarArray> IloNumVarArray2D;
    IloModel model;
    IloCplex cplex;

public:
    NOC_CPLEX();

    void write_LP(NOC *NoC);
    void CreateModel(IloModel model, NOC *NoC);
    int read_Sol(NOC *NoC, const char* Sol_file);
};

#endif //DIST_MILP_NOC_CPLEX_HPP
