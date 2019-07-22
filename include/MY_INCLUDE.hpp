//
// Created by tkhamvilai on 5/4/19.
//

#ifndef DIST_MILP_MY_INCLUDE_HPP
#define DIST_MILP_MY_INCLUDE_HPP

#include <iostream>
#include <csignal>
#include <fstream>
#include <unistd.h>
#include <vector>

#ifndef __x86_64__
#include <wiringPi.h>
#endif

#include <Eigen/Dense>

#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_MPI.hpp"
#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"
#include "ENGINE.hpp"
#include "MathHelperFunctions.hpp"

#endif //DIST_MILP_MY_INCLUDE_HPP
