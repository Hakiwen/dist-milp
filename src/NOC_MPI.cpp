//
// Created by tkhamvilai on 4/9/19.
//

#include "NOC_MPI.hpp"

NOC_MPI::NOC_MPI()
{
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &this->world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &this->world_rank);
}

void NOC_MPI::Barrier()
{
    MPI_Barrier(MPI_COMM_WORLD);
}

void NOC_MPI::Scatter_Apps(NOC *NoC) // TODO change to Allgather
{
    int *scatter_data_send = NULL;
    scatter_data_send = new int[NoC->N_CRs];
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        scatter_data_send[i] = NoC->nodes_on_CRs[i];
    }

    int *gather_data_receive = NULL;
    gather_data_receive = new int[this->world_size*NoC->N_CRs];

    MPI_Allgather(scatter_data_send, NoC->N_CRs, MPI_INT, gather_data_receive, NoC->N_CRs, MPI_INT, MPI_COMM_WORLD);

    int allocator_ind[NoC->allocator_app_num];
    for (int i = 0; i < NoC->allocator_app_num; i++)
    {
        allocator_ind[i] = -1;
    }

    int k = 0, ind = 0;
    for (int i = 0; i < this->world_size*NoC->N_CRs; i+=NoC->N_CRs)
    {
        int sum_nodes_on_CRs = 0;
        for (int j = 0; j < NoC->N_CRs; j++)
        {
            sum_nodes_on_CRs = sum_nodes_on_CRs + gather_data_receive[i+j];
        }
        if (sum_nodes_on_CRs != 0)
        {
            allocator_ind[ind] = k;
            ind++;
        }
        k++;
    }

    for (int i = 0; i < NoC->N_CRs; i++)
    {
        for (int j = 0; j < NoC->allocator_app_num; j++)
        {
            NoC->nodes_on_CRs_received(i,j) = gather_data_receive[allocator_ind[j]*NoC->N_CRs+i];
        }
    }

    if(world_rank > 0) // TODO voter
    {
        NoC->node_to_run = NoC->nodes_on_CRs_received(world_rank-1,0);

        if(NoC->app_to_run >= NoC->allocator_app_ind && NoC->app_to_run < NoC->allocator_app_ind + NoC->allocator_app_num)
        {
            for (int i = 0; i < NoC->N_CRs; i++)
            {
                for (int j = 0; j < NoC->N_nodes; j++)
                {
                    NoC->X_CRs_nodes_old(i, j) = 0;
                }
                if(NoC->nodes_on_CRs_received(i,0) > 0)
                {
                    NoC->X_CRs_nodes_old(i, NoC->nodes_on_CRs_received(i, 0) - 1) = 1;
                }
            }
        }
    }
}

void NOC_MPI::Gather_Internal_Faults(NOC *NoC)
{
    int *gather_data_receive = NULL;
    gather_data_receive = new int[this->world_size];
    MPI_Allgather(&NoC->fault_internal_status, 1, MPI_INT, gather_data_receive, 1, MPI_INT, MPI_COMM_WORLD);

    if(NoC->app_to_run >= NoC->allocator_app_ind && NoC->app_to_run < NoC->allocator_app_ind + NoC->allocator_app_num)
    {
        for (int i = 0; i < this->world_size - 1; i++)
        {
            NoC->Fault_Internal_CRs[i] = gather_data_receive[i + 1];
        }
    }
}

void NOC_MPI::Broadcast_External_Fault(ENGINE *Engine, NOC *NoC)
{
    MPI_Bcast(&Engine->fault_from_voter, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (NoC->app_to_run >= NoC->allocator_app_ind && NoC->app_to_run < NoC->allocator_app_ind + NoC->allocator_app_num)
    {
//        for (int i = 0; i < NoC->N_CRs; i++)
//        {
//            NoC->Fault_External_CRs[i] = 0;
//        }

        if (Engine->fault_from_voter > 0 && Engine->fault_from_voter <= NoC->N_CRs)
        {
            NoC->Fault_External_CRs[Engine->fault_from_voter - 1] = 1;
        }
    }
    else
    {
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            NoC->Fault_External_CRs[i] = 0;
        }
    }
}

void NOC_MPI::Broadcast_Sensor(ENGINE *Engine)
{
    MPI_Bcast(&Engine->sensor_data, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
}

void NOC_MPI::Gather_PWM(ENGINE *Engine)
{
    int *gather_data_receive = NULL;
    if(this->world_rank == 0)
    {
        gather_data_receive = new int[this->world_size];
    }

    MPI_Gather(&Engine->PWM_out, 1, MPI_INT, gather_data_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(this->world_rank == 0)
    {
        for (int i = 1; i < this->world_size - 1; i++)
        {
            Engine->PWM_in[i] = gather_data_receive[i + 1];
        }
    }

    Engine->PWM_out = 0;
}

void NOC_MPI::run(NOC *NoC, ENGINE *Engine)
{
#ifdef USE_MPI // TODO should be non-blocking
    this->Barrier();
    this->Broadcast_Sensor(Engine);
    this->Gather_PWM(Engine);
    this->Scatter_Apps(NoC);
    this->Gather_Internal_Faults(NoC);
    this->Broadcast_External_Fault(Engine, NoC);
    this->Barrier();
#endif
}