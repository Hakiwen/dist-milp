//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_APP_LED_HPP
#define DIST_MILP_APP_LED_HPP

#include "MY_MACROS.hpp"
#include "MY_INCLUDE.hpp"

#define LED_OFF -1
#define LED_WHITE 0
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3
#define LED_CYAN 4
#define LED_MAGENTA 5
#define LED_YELLOW 6

void APP_LED(NOC *NoC, NOC_FAULT *NoC_Fault, NOC_GLPK *NoC_GLPK, GLPK_SOLVER *prob_GLPK, ENGINE *Engine, int color);
void APP_LED_OFF();

#endif //DIST_MILP_APP_LED_HPP
