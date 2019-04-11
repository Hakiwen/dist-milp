//
// Created by tkhamvilai on 4/9/19.
//

#include "NOC_MPI.hpp"

NOC_MPI::NOC_MPI()
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &this->world_size);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &this->world_rank);
}

void NOC_MPI::Finalize()
{
    // Finalize the MPI environment.
    MPI_Finalize();
}

void NOC_MPI::Scatter_Apps(NOC *NoC)
{
    int *scatter_data_send = new int[NoC->N_CRs + 2];
    scatter_data_send[0] = 0;
    scatter_data_send[NoC->N_CRs + 1] = 0;
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        scatter_data_send[i+1] = NoC->apps_on_CRs[i];
    }
    int *scatter_data_receive = new int[1];
    MPI_Scatter(scatter_data_send, 1, MPI_INT, scatter_data_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
    NoC->app_to_run = scatter_data_receive[0];
}

void NOC_MPI::Gather_Faults(NOC *NoC)
{
    int *gather_data_receive = new int[NoC->N_CRs + 2];
    int *gather_data_send = new int[1];
    gather_data_send[0] = NoC->fault_status;
    MPI_Gather(gather_data_send, 1, MPI_INT, gather_data_receive, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        NoC->Fault_CRs[i] = gather_data_receive[i+1];
    }
}