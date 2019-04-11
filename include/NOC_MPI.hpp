//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_NOC_MPI_HPP
#define DIST_MILP_NOC_MPI_HPP

#include <iostream>
#include <mpi.h>
#include "NOC.hpp"

class NOC_MPI
{
public:
    NOC_MPI();

    int world_size;
    int world_rank;

    void Finalize();
    void Scatter_Apps(NOC *NoC);
    void Gather_Faults(NOC *NoC);
};

#endif //DIST_MILP_NOC_MPI_HPP
