//
// Created by tkhamvilai on 4/4/19.
//

#ifndef DIST_MILP_NOC_CPLEX_HPP
#define DIST_MILP_NOC_CPLEX_HPP

#include <iostream>
#include <csignal>

#include <Eigen/Dense>

#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>

#include "pugixml.hpp"

#include "NOC.hpp"

#define VERBOSE 1

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
