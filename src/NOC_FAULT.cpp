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
    this->fault_file << std::to_string(NoC->fault_internal_status_CR);
    this->fault_file.close();
#endif
}

int NOC_FAULT::Fault_Gathering(NOC *NoC)
{
    if (NoC->app_to_run >= NoC->allocator_app_ind && NoC->app_to_run < NoC->allocator_app_ind + NoC->allocator_app_num)
    {
        NoC->prev_N_Faults_CR = NoC->N_Faults_CR;
        NoC->prev_N_Faults_Paths = NoC->N_Faults_Paths;

#ifndef USE_MPI
        if(NoC->prev_N_Faults_CR != -1 || NoC->N_Faults_CR == 0)
        {
            /* user input for killing nodes */
            int fault_node = 0;
            std::cout << "Please input the valid number of node you want to kill: ";
            std::cin >> fault_node;
            NoC->Fault_Internal_CRs[fault_node - 1] = 1;
            /* end */
            NoC->prev_N_Faults_CR = 0;
        }
#endif
        NoC->CreateNeighborMatrixSquareTopology(); // update a Degree Matrix and Adjacency Matrix
        NoC->Find_Isolated_CRs(); // update Fault_Isolated

        std::vector<bool> path_to_fail(NoC->N_paths, false); // paths to fail from CRs
        std::vector<bool> CRs_to_fail(NoC->N_CRs, false); // CRs to fail from paths
        std::vector<bool> visited(NoC->N_paths, false);

        NoC->N_Faults_CR = 0;
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            if (NoC->Fault_Internal_CRs[i] || NoC->Fault_Isolated_CRs[i]) // neighbor paths fail if the node fails
            {
                for (int j = 0; j < NoC->N_paths; j++)
                {
                    if (abs(NoC->G(i, j)) && !visited[j])
                    {
                        visited[j] = true;
                        path_to_fail[j] = true; // update paths to fail from internal & isolated faulty nodes
                    }
                }
            }
        }

        for (int i = 0; i < NoC->N_CRs; i++)
        {
            for (int j = 0; j < NoC->N_paths; j++)
            {
                if(NoC->G(i,j))
                {
                    CRs_to_fail[i] = (NoC->Fault_Paths_receive[j] || path_to_fail[j]) && NoC->G(i, j);
                    if(!CRs_to_fail[i])
                    {
                        break;
                    }
                }
            }

            int prev_Fault = NoC->Fault_CRs[i];
            NoC->Fault_CRs[i] = NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i] || NoC->Fault_Isolated_CRs[i] || CRs_to_fail[i];
            if(NoC->Fault_CRs[i] != prev_Fault)
            {
                NoC->N_Faults_CR += 1;
            }
        }

        NoC->N_Faults_Paths = 0;
        for (int i = 0; i < NoC->N_paths; i++)
        {
            int prev_Fault = NoC->Fault_Paths[i];
            NoC->Fault_Paths[i] = NoC->Fault_Paths_receive[i] || path_to_fail[i];
            if(NoC->Fault_Paths[i] != prev_Fault)
            {
                NoC->N_Faults_Paths += 1;
            }
        }

        if(NoC->N_Faults_CR == NoC->prev_N_Faults_CR && NoC->N_Faults_Paths == NoC->prev_N_Faults_Paths)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    return 0;
}

int NOC_FAULT::Fault_Detection(NOC *NoC, int rank) // detect fault from the switches
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
        NoC->fault_internal_status_CR = (int)buf[0] - '0';
        delete[] buf;
#else
        int switch_button_1 = NODE_FAULT_PIN;
        pinMode (switch_button_1, INPUT);
        pullUpDnControl (switch_button_1, PUD_UP);
        if(digitalRead (switch_button_1) == 1)
        {
            NoC->fault_internal_status_CR = 0;
        }
        else
        {
            NoC->fault_internal_status_CR = 1;
        }
#endif
        return NoC->fault_internal_status_CR;
    }
    return 0;
}