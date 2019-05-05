//
// Created by tkhamvilai on 4/16/19.
//

#ifndef DIST_MILP_APP_PID_HPP
#define DIST_MILP_APP_PID_HPP

#include "MY_MACROS.hpp"
#include "MY_INCLUDE.hpp"

#include "APP_LED.hpp"

void APP_PID(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color);
void APP_PWM_OFF();

#endif //DIST_MILP_APP_PID_HPP
