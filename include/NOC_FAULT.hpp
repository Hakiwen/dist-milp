//
// Created by tkhamvilai on 4/7/19.
//

#ifndef DIST_MILP_NOC_FAULT_HPP
#define DIST_MILP_NOC_FAULT_HPP

#include <iostream>
#include "NOC.hpp"

class NOC_FAULT
{
public:
    NOC_FAULT();

    int Fault_Detection(NOC *NoC); // user input for now
};

#endif //DIST_MILP_NOC_FAULT_HPP
