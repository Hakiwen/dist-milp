//
// Created by tkhamvilai on 7/22/19.
//

#ifndef DIST_MILP_APP_PATH_HPP
#define DIST_MILP_APP_PATH_HPP

#include "MY_MACROS.hpp"
#include "MY_INCLUDE.hpp"

#define LED_PATH_OFF -1
#define LED_PATH_WHITE 0 // 000
#define LED_PATH_RED 1 // 001
#define LED_PATH_GREEN 2 // 010
#define LED_PATH_RED_GREEN 3 // 011
#define LED_PATH_BLUE 4 // 100
#define LED_PATH_RED_BLUE 5 // 101
#define LED_PATH_GREEN_BLUE 6 // 110
#define LED_PATH_RED_GREEN_BLUE 7 // 111

void PATH_LED(int* path_to_run, Eigen::MatrixXi G, int rank);
void PATH_LED_OFF();

#endif //DIST_MILP_APP_PATH_HPP
