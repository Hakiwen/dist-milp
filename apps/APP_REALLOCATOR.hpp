//
// Created by tkhamvilai on 5/4/19.
//

#ifndef DIST_MILP_APP_REALLOCATOR_HPP
#define DIST_MILP_APP_REALLOCATOR_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"
#include "APP_LED.hpp"

void APP_REALLOCATOR(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color);

#endif //DIST_MILP_APP_REALLOCATOR_HPP
