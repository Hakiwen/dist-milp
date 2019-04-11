//
// Created by tkhamvilai on 4/7/19.
//

#include "NOC_FAULT.hpp"

NOC_FAULT::NOC_FAULT(NOC *NoC, int rank)
{
    std::string name = "fault_file_" + std::to_string(rank) + ".txt";
    this->fault_file.open(name, std::fstream::out);
    this->fault_file.clear();
    this->fault_file << std::to_string(NoC->fault_status);
    this->fault_file.close();
}

int NOC_FAULT::Fault_Detection(NOC *NoC, int rank)
{
    if (rank == 0)
    {
//        int fault_node = 0;
//        std::cout << "Please input the valid number of node you want to kill: ";
//        std::cin >> fault_node;
//        NoC->Fault_CRs[fault_node - 1] = 1;

        std::cout << "All Nodes' Status: ";
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            std::cout << NoC->Fault_CRs[i];
        }
        std::cout << std::endl;

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
    else
    {
        // Read file instead of user input so that the loop can keep running
        std::string name = "fault_file_" + std::to_string(rank) + ".txt";
        this->fault_file.open(name, std::fstream::in);
        char *buf = new char[1];
        this->fault_file.read(buf, 1);
        this->fault_file.close();
        NoC->fault_status = (int)buf[0] - '0';
        delete[] buf;
    }
}