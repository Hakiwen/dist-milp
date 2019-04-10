//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_APP_LED_HPP
#define DIST_MILP_APP_LED_HPP

#include <iostream>
#include <wiringPi.h>

#define LED_OFF -1
#define LED_WHITE 0
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3

void APP_LED(int color);

#endif //DIST_MILP_APP_LED_HPP
