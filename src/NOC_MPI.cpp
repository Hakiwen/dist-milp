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

void Finalize()
{
    // Finalize the MPI environment.
    MPI_Finalize();
}