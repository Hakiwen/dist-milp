//
// Created by tkhamvilai on 4/7/19.
//

#ifndef DIST_MILP_NOC_FAULT_HPP
#define DIST_MILP_NOC_FAULT_HPP

#include "MY_MACROS.hpp"
#include "MathHelperFunctions.hpp"
#include <iostream>
#include <fstream>
#include <vector>

#ifndef __x86_64__
#include <wiringPi.h>
#endif

#include "NOC.hpp"

class NOC_FAULT
{
public:
    std::fstream fault_file;

    NOC_FAULT(NOC *NoC, int rank);

    void Fault_Isolated_Update(NOC *NoC);
    void Find_Isolated_Sets(NOC *NoC);
    void Find_Isolated_CRs_by_max_nodes(NOC *NoC);
    void Find_Isolated_CRs_by_max_alloc(NOC *NoC);
    int Find_Alloc_Num_in_Set(NOC *NoC, std::vector<int> set);
    int Fault_Gathering(NOC *NoC); // allocators nodes
    int Fault_Detection(NOC *NoC, int rank); // all CRs
};

#endif //DIST_MILP_NOC_FAULT_HPP
