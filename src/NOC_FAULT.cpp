//
// Created by tkhamvilai on 4/7/19.
//

#include "NOC_FAULT.hpp"

NOC_FAULT::NOC_FAULT(NOC *NoC)
{
    this->fault_file.open("fault_file.txt", std::fstream::out);
    this->fault_file.clear();
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        this->fault_file << std::to_string(NoC->Fault_CRs[i]);
    }
    this->fault_file.close();
}

int NOC_FAULT::Fault_Detection(NOC *NoC)
{
//    int fault_node = 0;
//    std::cout << "Please input the valid number of node you want to kill: ";
//    std::cin >> fault_node;
//    NoC->Fault_CRs[fault_node - 1] = 1;

    // Read file instead of user input so that the loop can keep running
    this->fault_file.open("fault_file.txt", std::fstream::in);
    char *buf = new char[NoC->N_CRs];
    this->fault_file.read(buf, NoC->N_CRs);
    this->fault_file.close();
    std::cout << "Nodes' Status: ";
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        NoC->Fault_CRs[i] = (int)buf[i] - '0';
        std::cout << NoC->Fault_CRs[i];
    }
    std::cout << std::endl;
    delete[] buf;

    NoC->prev_N_Faults = NoC->N_Faults;
    NoC->N_Faults = 0;
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        if(NoC->Fault_CRs[i] == 1)
        {
            NoC->N_Faults += 1;
        }
    }

    if(NoC->N_Faults == NoC->prev_N_Faults)
    {
        std::cout << "No New Faults Occurred, Do Nothing" << std::endl;
        return 0;
    }
    else
    {
        if(NoC->N_Faults == 0 && NoC->prev_N_Faults == -1)
        {
            std::cout << "First Reallocation..." << std::endl;
            return 1;
        }
        else
        {
            std::cout << "A New Fault Occurred, Reallocating..." << std::endl;
            return 1;
        }
    }
}