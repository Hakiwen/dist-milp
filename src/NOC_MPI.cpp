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

void NOC_MPI::Finalize()
{
    MPI_Finalize();
}

void NOC_MPI::Scatter_Apps(NOC *NoC)
{
    int *scatter_data_send = NULL;
    if(this->world_rank == 0)
    {
        scatter_data_send = new int[NoC->N_CRs + 2];
        scatter_data_send[0] = 0;
        scatter_data_send[NoC->N_CRs + 1] = 0;
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            scatter_data_send[i + 1] = NoC->nodes_on_CRs[i];
        }
    }

    MPI_Scatter(scatter_data_send, 1, MPI_INT, &NoC->node_to_run, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void NOC_MPI::Gather_Faults(NOC *NoC)
{
    int *gather_data_receive = NULL;
    if(this->world_rank == 0)
    {
        gather_data_receive = new int[this->world_size];
    }

    MPI_Gather(&NoC->fault_internal_status, 1, MPI_INT, gather_data_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(this->world_rank == 0)
    {
        for (int i = 0; i < this->world_size - 2; i++)
        {
            NoC->Fault_Internal_CRs[i] = gather_data_receive[i + 1];
        }
    }
}

void NOC_MPI::Broadcast_Sensor(ENGINE *Engine)
{
    MPI_Bcast(&Engine->sensor_data, 1, MPI_FLOAT, this->world_size - 1, MPI_COMM_WORLD);
}

void NOC_MPI::Gather_PWM(ENGINE *Engine)
{
    int *gather_data_receive = NULL;
    if(this->world_rank == this->world_size - 1)
    {
        gather_data_receive = new int[this->world_size];
    }

    MPI_Gather(&Engine->PWM_out, 1, MPI_INT, gather_data_receive, 1, MPI_INT, this->world_size - 1, MPI_COMM_WORLD);

    if(this->world_rank == this->world_size - 1)
    {
        for (int i = 0; i < this->world_size - 2; i++)
        {
            Engine->PWM_in[i] = gather_data_receive[i + 1];
        }
    }

    Engine->PWM_out = 0;
}