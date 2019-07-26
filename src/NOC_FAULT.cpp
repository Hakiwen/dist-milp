//
// Created by tkhamvilai on 4/7/19.
//

#include <MathHelperFunctions.hpp>
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

void NOC_FAULT::Fault_Isolated_Update(NOC *NoC)
{
    // Two lines below are updated from both internal and external faults
    NoC->CreateNeighborMatrixSquareTopology(); // update a Degree Matrix and Adjacency Matrix
    this->Find_Isolated_Sets(NoC); // update Isolated sets
//    this->Find_Isolated_CRs_by_max_nodes(NoC); // update Fault_Isolated
    this->Find_Isolated_CRs_by_max_alloc(NoC); // update Fault_Isolated
}

void NOC_FAULT::Find_Isolated_Sets(NOC *NoC)
{
//    std::cout << "Disconnected: " << std::endl;
//    std::cout << A << std::endl;
    std::vector<bool> visited(NoC->N_CRs, false);
    std::vector<int> disconnected_set;

    NoC->disconnected_sets.clear();
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        if (visited[i] == false && (NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i]) == 0)
        {
            std::list<int> q;
            visited[i] = true;
            q.push_back(i);

            while (!q.empty())
            {
                int j = q.front();
                disconnected_set.push_back(j + 1);
                q.pop_front();

                for (int k = 0; k < NoC->N_CRs; k++)
                {
                    if (visited[k] == false && NoC->A(j, k) == 1)
                    {
                        visited[k] = true;
                        q.push_back(k);
                    }
                }
            }

            NoC->disconnected_sets.push_back(disconnected_set);
            disconnected_set.clear();
        }
    }

//    std::cout <<  "disconnected_sets: " << std::endl;
//    for (unsigned int i = 0; i < NoC->disconnected_sets.size(); i++)
//    {
//        for (unsigned int j = 0; j < NoC->disconnected_sets[i].size(); j++)
//        {
//            std::cout << NoC->disconnected_sets[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
}

void NOC_FAULT::Find_Isolated_CRs_by_max_nodes(NOC *NoC)
{
    if (NoC->Fault_Isolated_CRs_ind.empty())
    {
        if (NoC->disconnected_sets.size() > 1)
        {
            int max_disconnected_set = 0;
            for (unsigned int i = 1; i < NoC->disconnected_sets.size(); i++)
            {
                if (NoC->disconnected_sets[i].size() > NoC->disconnected_sets[max_disconnected_set].size())
                {
                    for (unsigned int j = 0; j < NoC->disconnected_sets[max_disconnected_set].size(); j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[max_disconnected_set][j]);
                    }
                    max_disconnected_set = i;
                }
                else
                {
                    for (unsigned int j = 0; j < NoC->disconnected_sets[i].size(); j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[i][j]);
                    }
                }
            }
        }
    }
    else //if (this->disconnected_sets.size() >= 2)
    {
        std::vector<int> disconnected_set_not_in_fault, disconnected_set_not_in_fault_size;
        for (unsigned int i = 0; i < NoC->disconnected_sets.size(); i++)
        {
            if (!MathHelperFunctions_isSubset(NoC->Fault_Isolated_CRs_ind, NoC->disconnected_sets[i]))
            {
                disconnected_set_not_in_fault.push_back(i); // stores the indices for disconnected sets that are not in fault_isolated
                disconnected_set_not_in_fault_size.push_back(NoC->disconnected_sets[i].size()); // stores size of that index
            }
        }

        int max_val = *std::max_element(disconnected_set_not_in_fault_size.begin(), disconnected_set_not_in_fault_size.end());
        int is_break_tie = 0;
        for (unsigned int i = 0; i < disconnected_set_not_in_fault.size(); i++)
        {
            if(disconnected_set_not_in_fault_size[i] != max_val || is_break_tie == 1) // the isolated cases
            {
                for (unsigned int j = 0; j < NoC->disconnected_sets[disconnected_set_not_in_fault[i]].size(); j++)
                {
                    NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[disconnected_set_not_in_fault[i]][j]);
                }
            }
            else if(is_break_tie == 0) // the case we want to keep
            {
                is_break_tie = 1;
                for (int j = 0; j < disconnected_set_not_in_fault_size[i]; j++)
                {
                    NoC->Fault_Isolated_CRs_ind.erase(std::remove(NoC->Fault_Isolated_CRs_ind.begin(), NoC->Fault_Isolated_CRs_ind.end(), NoC->disconnected_sets[disconnected_set_not_in_fault[i]][j]), NoC->Fault_Isolated_CRs_ind.end());
                }
            }
        }

        for (int i = 0; i < NoC->N_CRs; i++)
        {
            if(NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i])
            {
                NoC->Fault_Isolated_CRs_ind.erase(std::remove(NoC->Fault_Isolated_CRs_ind.begin(), NoC->Fault_Isolated_CRs_ind.end(), i+1), NoC->Fault_Isolated_CRs_ind.end());
            }
            NoC->Fault_Isolated_CRs[i] = 0;
        }
    }

//    std::cout << "Fault_Isolated: " << std::endl;
//    for (unsigned int i = 0; i < NoC->Fault_Isolated_CRs_ind.size(); i++)
//    {
//        std::cout << NoC->Fault_Isolated_CRs_ind[i] << " ";
//    }
//    std::cout << std::endl;

    for (unsigned int i = 0; i < NoC->Fault_Isolated_CRs_ind.size(); i++)
    {
        NoC->Fault_Isolated_CRs[NoC->Fault_Isolated_CRs_ind[i] - 1] = 1;
    }
}

int NOC_FAULT::Find_Alloc_Num_in_Set(NOC *NoC, std::vector<int> set)
{
    std::vector<int> alloc_nodes_in_CRs;
    for (int k = 0; k < NoC->allocator_app_num; k++)
    {
        for (int i = 0; i < NoC->N_CRs; i++)
        {
#ifdef USE_MPI
            if(NoC->X_CRs_nodes_received(i, NoC->allocator_nodes_ind[k]-1) == 1)
#else
            if(NoC->X_CRs_nodes(i, NoC->allocator_nodes_ind[k]-1) == 1)
#endif
            {
                alloc_nodes_in_CRs.push_back(i+1);
                break;
            }
            else // app was dropped
            {

            }
        }
    }
    return MathHelperFunctions_Intersection(alloc_nodes_in_CRs, set);
}

void NOC_FAULT::Find_Isolated_CRs_by_max_alloc(NOC *NoC)
{
    std::vector<int> num_alloc_in_set;
    if (NoC->Fault_Isolated_CRs_ind.empty())
    {
        if (NoC->disconnected_sets.size() > 1)
        {
            int max_alloc_set = 0;
            for (unsigned int i = 1; i < NoC->disconnected_sets.size(); i++)
            {
                if (Find_Alloc_Num_in_Set(NoC, NoC->disconnected_sets[i]) > Find_Alloc_Num_in_Set(NoC, NoC->disconnected_sets[max_alloc_set]))
                {
                    for (unsigned int j = 0; j < NoC->disconnected_sets[max_alloc_set].size(); j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[max_alloc_set][j]);
                    }
                    max_alloc_set = i;
                }
                else
                {
                    for (unsigned int j = 0; j < NoC->disconnected_sets[i].size(); j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[i][j]);
                    }
                }
            }
        }
    }
    else //if (this->disconnected_sets.size() >= 2)
    {
        std::vector<unsigned int> disconnected_set_not_in_fault, disconnected_set_not_in_fault_size, disconnected_set_not_in_alloc_num;
        for (unsigned int i = 0; i < NoC->disconnected_sets.size(); i++)
        {
            if (!MathHelperFunctions_isSubset(NoC->Fault_Isolated_CRs_ind, NoC->disconnected_sets[i]))
            {
                disconnected_set_not_in_fault.push_back(i); // stores the indices for disconnected sets that are not in fault_isolated
                disconnected_set_not_in_alloc_num.push_back(Find_Alloc_Num_in_Set(NoC, NoC->disconnected_sets[i])); // stores alloc num of that index
                disconnected_set_not_in_fault_size.push_back(0);
            }
        }

        unsigned int is_break_tie = 0;
        unsigned int max_alloc = *std::max_element(disconnected_set_not_in_alloc_num.begin(), disconnected_set_not_in_alloc_num.end());

        for (unsigned int i = 0; i < disconnected_set_not_in_fault.size(); i++)
        {
            if (disconnected_set_not_in_alloc_num[i] == max_alloc)
            {
                disconnected_set_not_in_fault_size.at(i) = (NoC->disconnected_sets[disconnected_set_not_in_fault[i]].size()); // stores size of the tied allocs
            }
        }
        unsigned int max_size = *std::max_element(disconnected_set_not_in_fault_size.begin(), disconnected_set_not_in_fault_size.end()); // max size among tied allocs

        for (unsigned int i = 0; i < disconnected_set_not_in_fault.size(); i++)
        {
            if(disconnected_set_not_in_alloc_num[i] != max_alloc || is_break_tie == 1) // the isolated cases
            {
                for (unsigned int j = 0; j < NoC->disconnected_sets[disconnected_set_not_in_fault[i]].size(); j++)
                {

                    NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[disconnected_set_not_in_fault[i]][j]);
                }
            }
            else if (disconnected_set_not_in_alloc_num[i] == max_alloc)
            {
                if (NoC->disconnected_sets[disconnected_set_not_in_fault[i]].size() == max_size && is_break_tie == 0)
                {
                    is_break_tie = 1;
                    for (unsigned int j = 0; j < disconnected_set_not_in_fault_size[i]; j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.erase(std::remove(NoC->Fault_Isolated_CRs_ind.begin(), NoC->Fault_Isolated_CRs_ind.end(), NoC->disconnected_sets[disconnected_set_not_in_fault[i]][j]), NoC->Fault_Isolated_CRs_ind.end());
                    }
                }
                else
                {
                    for (unsigned int j = 0; j < NoC->disconnected_sets[disconnected_set_not_in_fault[i]].size(); j++)
                    {
                        NoC->Fault_Isolated_CRs_ind.push_back(NoC->disconnected_sets[disconnected_set_not_in_fault[i]][j]);
                    }
                }
            }
        }

        for (int i = 0; i < NoC->N_CRs; i++)
        {
            if(NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i]) // if the fault is already in internal/external, remove it from isolated
            {
                NoC->Fault_Isolated_CRs_ind.erase(std::remove(NoC->Fault_Isolated_CRs_ind.begin(), NoC->Fault_Isolated_CRs_ind.end(), i+1), NoC->Fault_Isolated_CRs_ind.end());
            }
            NoC->Fault_Isolated_CRs[i] = 0;
        }
    }

//    std::cout << "Fault_Isolated: " << std::endl;
//    for (unsigned int i = 0; i < NoC->Fault_Isolated_CRs_ind.size(); i++)
//    {
//        std::cout << NoC->Fault_Isolated_CRs_ind[i] << " ";
//    }
//    std::cout << std::endl;

    for (unsigned int i = 0; i < NoC->Fault_Isolated_CRs_ind.size(); i++)
    {
        NoC->Fault_Isolated_CRs[NoC->Fault_Isolated_CRs_ind[i] - 1] = 1;
    }
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
            std::cout << "Please input the valid (negative/positive) number of node to kill/recover: ";
            std::cin >> fault_node;
            if(fault_node > 0 && fault_node <= NoC->N_CRs)
            {
                NoC->Fault_Internal_CRs[abs(fault_node) - 1] = 0;
            }
            else if(fault_node < 0 && fault_node >= NoC->N_CRs*(-1))
            {
                NoC->Fault_Internal_CRs[abs(fault_node) - 1] = 1;
            }
            else
            {
                std::cout << "Invalid Number, Try again" << std::endl;
            }
            /* end */
            NoC->prev_N_Faults_CR = 0;
        }
        Fault_Isolated_Update(NoC);
#endif
        std::vector<bool> path_to_fail(NoC->N_paths, false); // paths to fail from CRs
        std::vector<bool> CRs_to_fail(NoC->N_CRs, false); // CRs to fail from paths
        std::vector<bool> visited(NoC->N_paths, false);

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

        NoC->N_Faults_CR = 0;
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
            NoC->Fault_CRs[i] = NoC->Fault_Internal_CRs[i] || NoC->Fault_External_CRs[i] || NoC->Fault_Isolated_CRs[i] || CRs_to_fail[i];
            if(NoC->Fault_CRs[i])
            {
                NoC->N_Faults_CR += 1;
            }
//            std::cout << NoC->Fault_Internal_CRs[i];
        }

        NoC->N_Faults_Paths = 0;
        for (int i = 0; i < NoC->N_paths; i++)
        {
            NoC->Fault_Paths[i] = NoC->Fault_Paths_receive[i] || path_to_fail[i];
//            std::cout << NoC->Fault_Paths[i];
            if(NoC->Fault_Paths[i])
            {
                NoC->N_Faults_Paths += 1;
            }
        }
//        std::cout << std::endl;

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
        if (digitalRead (switch_button_1) == 1)
        {
            NoC->fault_internal_status_CR = 0;
        }
        else
        {
            NoC->fault_internal_status_CR = 1;
        }

        int switch_button_path[NoC->N_neighbors] = {PATH_FAULT_PIN_1, PATH_FAULT_PIN_2, PATH_FAULT_PIN_3, PATH_FAULT_PIN_4};
        int switch_button_status[NoC->N_neighbors] = {0, 0, 0, 0}; // alive at the beginning
        for (int i = 0; i < NoC->N_neighbors; i++)
        {
            pinMode (switch_button_path[i], INPUT);
            pullUpDnControl (switch_button_path[i], PUD_UP);
            if (digitalRead (switch_button_path[i]) == 1)
            {
                switch_button_status[i] = 0;
            }
            else
            {
                switch_button_status[i] = 1;
            }
//            if (rank == 1) std::cout << switch_button_status[i];
        }
//        if (rank == 1) std::cout << std::endl;

        int path_ind[NoC->N_neighbors], UP_IND = 0, DOWN_IND = 1, LEFT_IND = 2, RIGHT_IND = 3;
        int i = rank - 1, N_Col = NoC->N_Col_CRs, N_Row = NoC->N_Row_CRs;
        int N_joint = (N_Col - 1)*N_Row + (N_Row - 1)*N_Col;
        int current_row = int(i / N_Col);
        path_ind[UP_IND] = (i+1) - N_Col + (N_Col - 1)*current_row;
        path_ind[DOWN_IND] = (i+1) + (N_Col - 1)*(current_row + 1);
        path_ind[LEFT_IND] = (i+1) - N_Col + (N_Col - 1)*(current_row + 1);
        path_ind[RIGHT_IND] = (i+1) + (N_Col - 1)*current_row;

        for (int j = 0; j < NoC->N_neighbors; j++)
        {
            if(path_ind[j] > 0 && path_ind[j] <= N_joint)
            {
                NoC->Fault_Paths_send[path_ind[j] - 1] = switch_button_status[j];
            }
        }

        if(N_Col > 1)
        {
            if ((i + 1) % N_Col == 1) // first column
            {
                if (path_ind[LEFT_IND] > 0 && path_ind[LEFT_IND] <= N_joint)
                {
                    NoC->Fault_Paths_send[path_ind[LEFT_IND] - 1] = -1;
                }
            }
            else if ((i + 1) % N_Col == 0) // last column
            {
                if (path_ind[RIGHT_IND] > 0 && path_ind[RIGHT_IND] <= N_joint)
                {
                    NoC->Fault_Paths_send[path_ind[RIGHT_IND] - 1] = -1;
                }
            }
        }

//        for(int j = 0; j < NoC->N_paths; j++)
//        {
//            if (rank == 2) std::cout << NoC->Fault_Paths_send[j] << ", ";
//        }
//        if (rank == 2) std::cout << std::endl;
#endif
#ifdef USE_MPI
        Fault_Isolated_Update(NoC);
#endif
        return NoC->fault_internal_status_CR;
    }
    return 0;
}