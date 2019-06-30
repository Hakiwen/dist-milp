//
// Created by tkhamvilai on 4/7/19.
//

#ifndef DIST_MILP_NOC_FAULT_HPP
#define DIST_MILP_NOC_FAULT_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <fstream>

#ifndef __x86_64__
#include <wiringPi.h>
#endif

#include "NOC.hpp"

class NOC_FAULT
{
public:
    std::fstream fault_file;

    NOC_FAULT(NOC *NoC, int rank);

    int Fault_Gathering(NOC *NoC); // allocators nodes
    int Fault_Detection(NOC *NoC, int rank); // all CRs
};

#endif //DIST_MILP_NOC_FAULT_HPP
