//
// Created by tkhamvilai on 4/9/19.
//

#ifndef DIST_MILP_NOC_MPI_HPP
#define DIST_MILP_NOC_MPI_HPP

#include "MY_MACROS.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <mpi.h>
#include "NOC.hpp"
#include "ENGINE.hpp"

class NOC_MPI
{
public:
    NOC_MPI();

    int world_size;
    int world_rank;

    void Barrier(); // control barrier function

    void Scatter_Apps(NOC *NoC);
    void Scatter_Paths(NOC *NoC);
    void Gather_Internal_Faults(NOC *NoC);
    void Broadcast_External_Fault(ENGINE *Engine, NOC *NoC);
    void Gather_Path_Faults(NOC *NoC);
    void Broadcast_Sensor(ENGINE *Engine);
    void Gather_PWM(ENGINE *Engine);

    void run(NOC *NoC, ENGINE *Engine);
};

#endif //DIST_MILP_NOC_MPI_HPP
