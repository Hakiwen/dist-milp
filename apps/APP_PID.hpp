//
// Created by tkhamvilai on 4/16/19.
//

#ifndef DIST_MILP_APP_PID_HPP
#define DIST_MILP_APP_PID_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include "NOC.hpp"
#include "ENGINE.hpp"
#include "APP_LED.hpp"
#include "wiringPi.h"

void APP_PID(NOC *NoC, ENGINE *Engine, int color);
void APP_PWM_OFF();

#endif //DIST_MILP_APP_PID_HPP
