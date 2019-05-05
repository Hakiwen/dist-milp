//
// Created by tkhamvilai on 5/4/19.
//

#ifndef DIST_MILP_MY_INCLUDE_HPP
#define DIST_MILP_MY_INCLUDE_HPP

#include <iostream>
#include <csignal>
#include <fstream>

#include <wiringPi.h>
#include <Eigen/Dense>

#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_MPI.hpp"
#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"
#include "ENGINE.hpp"

typedef struct
{
    NOC NoC;
    NOC_MPI NoC_MPI;
    NOC_FAULT NoC_Fault;
    NOC_GLPK NoC_GLPK;
    GLPK_SOLVER prob_GLPK;
    ENGINE Engine;
} NOC_TYPE;

#endif //DIST_MILP_MY_INCLUDE_HPP
