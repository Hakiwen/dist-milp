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
        gather_data_receive = new int[NoC->N_CRs + 2];
    }

    MPI_Gather(&NoC->fault_status, 1, MPI_INT, gather_data_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(this->world_rank == 0)
    {
        for (int i = 0; i < NoC->N_CRs; i++)
        {
            NoC->Fault_CRs[i] = gather_data_receive[i + 1];
        }
    }
}