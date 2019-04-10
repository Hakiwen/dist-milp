//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_NOC_MPI_HPP
#define DIST_MILP_NOC_MPI_HPP

#include <iostream>
#include <mpi.h>

class NOC_MPI
{
public:
    NOC_MPI();

    int world_size;
    int world_rank;

    void Finalize();
};

#endif //DIST_MILP_NOC_MPI_HPP
