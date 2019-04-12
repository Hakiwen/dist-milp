//
// Created by tkhamvilai on 4/7/19.
//

#ifndef DIST_MILP_NOC_FAULT_HPP
#define DIST_MILP_NOC_FAULT_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <fstream>

#include "NOC.hpp"

class NOC_FAULT
{
public:
    std::fstream fault_file;

    NOC_FAULT(NOC *NoC, int rank);

    int Fault_Detection(NOC *NoC, int rank);// user input for now
};

#endif //DIST_MILP_NOC_FAULT_HPP
