//
// Created by tkhamvilai on 4/7/19.
//

#include "NOC_FAULT.hpp"

NOC_FAULT::NOC_FAULT(NOC *NoC, int rank)
{
#if defined(__x86_64__)
    std::string name = "fault_file_" + std::to_string(rank) + ".txt";
    this->fault_file.open(name, std::fstream::out);
    this->fault_file.clear();
    this->fault_file << std::to_string(NoC->fault_internal_status);
    this->fault_file.close();
#endif
}

int NOC_FAULT::Fault_Gathering(NOC *NoC)
{
    if (NoC->app_to_run >= NoC->allocator_app_ind && NoC->app_to_run < NoC->allocator_app_ind + NoC->allocator_app_num)
    {
        NoC->prev_N_Faults = NoC->N_Faults;

#ifndef USE_MPI
        if(NoC->prev_N_Faults != -1 || NoC->N_Faults == 0)
        {
            /* user input for killing nodes */
            int fault_node = 0;
            std::cout << "Please input the valid number of node you want to kill: ";
            std::cin >> fault_node;
            NoC->Fault_Internal_CRs[fault_node - 1] = 1;
            /* end */
            NoC->prev_N_Faults = 0;
        }
#endif

        NoC->N_Faults = 0;
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            int prev_Fault = NoC->Fault_CRs[i];
            NoC->Fault_CRs[i] = NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i];
            if(NoC->Fault_CRs[i] != prev_Fault)
            {
                NoC->N_Faults += 1;
            }
        }

        if(NoC->N_Faults == NoC->prev_N_Faults)
        {
//            std::cout << "No New Node Fails/Recovers, Do Nothing" << std::endl;
            return 0;
        }
        else
        {
//            if(NoC->N_Faults == 0 && NoC->prev_N_Faults == -1)
//            {
//                std::cout << "First Reallocation..." << std::endl;
//            }
//            else
//            {
//                std::cout << "A New Node Fails/Recovers, Reallocating..." << std::endl;
//            }
            return 1;
        }
    }
}

int NOC_FAULT::Fault_Detection(NOC *NoC, int rank)
{
    if (rank >= 1 && rank <= NoC->N_CRs)
    {
#if defined(__x86_64__)
        // Read file instead of user input so that the loop can keep running
        std::string name = "fault_file_" + std::to_string(rank) + ".txt";
        this->fault_file.open(name, std::fstream::in);
        char *buf = new char[1];
        this->fault_file.read(buf, 1);
        this->fault_file.close();
        NoC->fault_internal_status = (int)buf[0] - '0';
        delete[] buf;
#else
        int switch_button_1 = 29;
        pinMode (switch_button_1, INPUT);
        pullUpDnControl (switch_button_1, PUD_UP);
        if(digitalRead (switch_button_1) == 1)
        {
            NoC->fault_internal_status = 0;
        }
        else
        {
            NoC->fault_internal_status = 1;
        }
#endif
        return 1;
    }
    return 1;
}