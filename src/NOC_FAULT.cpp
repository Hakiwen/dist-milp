//
// Created by tkhamvilai on 4/7/19.
//

#include "NOC_FAULT.hpp"

NOC_FAULT::NOC_FAULT()
{

}

int NOC_FAULT::Fault_Detection(NOC *NoC)
{
    int fault_node = 0;
    std::cout << "Please input the valid number of node you want to kill: ";
    std::cin >> fault_node;

    NoC->Fault_CRs[fault_node - 1] = 1;

    NoC->N_Faults = 0;
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        if(NoC->Fault_CRs[i] == 1)
        {
            NoC->N_Faults += 1;
        }
    }

    return 1;
}