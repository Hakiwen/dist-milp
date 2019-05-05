//
// Created by tkhamvilai on 4/16/19.
//

#ifndef DIST_MILP_APP_PID_HPP
#define DIST_MILP_APP_PID_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"
#include "ENGINE.hpp"
#include "APP_LED.hpp"
#include "wiringPi.h"

void APP_PID(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color);
void APP_PWM_OFF();

#endif //DIST_MILP_APP_PID_HPP
