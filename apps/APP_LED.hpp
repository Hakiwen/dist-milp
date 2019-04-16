//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_APP_LED_HPP
#define DIST_MILP_APP_LED_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <wiringPi.h>
#include "NOC.hpp"
#include "ENGINE.hpp"

#define LED_OFF -1
#define LED_WHITE 0
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3

void APP_LED(NOC *NoC, ENGINE *Engine);
void APP_LED_WHITE(NOC *NoC, ENGINE *Engine);
void APP_LED_OFF();

#endif //DIST_MILP_APP_LED_HPP
