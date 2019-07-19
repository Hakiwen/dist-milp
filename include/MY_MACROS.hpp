//
// Created by tkhamvilai on 4/12/19.
//

#ifndef DIST_MILP_MY_MACROS_HPP
#define DIST_MILP_MY_MACROS_HPP

#define VERBOSE 0
#define PRINT 1

#define COMPUTATIONAL_FAULT_PIN 25 // external fault, only the node fails (running, but gives wrong results), its neighbor paths are still alive
#define RED_LED_PIN 26
#define GREEN_LED_PIN 27
#define BLUE_LED_PIN 28
#define NODE_FAULT_PIN 29 // internal fault , the node and its neighbor are completely fail

#define USE_MPI

//// Choose one, comment others ////
//#define CPLEX_AS_SOLVER
#define GLPK_AS_SOLVER

//// Choose one, comment others ////
//#define USE_ENGINE_W_FEEDBACK
//#define USE_ENGINE_WO_FEEDBACK
#define USE_X_PLANE_SIMULATOR

#ifdef USE_X_PLANE_SIMULATOR
#define X_PLANE_IP_ADDRESS "192.168.0.99"
#define X_PLANE_PORT 49000
#define X_PLANE_MAX_BYTE 1024
#define X_LANE BUFFER_SIZE 40
#define X_PLANE_PACKET_BYTE 113 // dependong on what data you want to received
#endif

#endif //DIST_MILP_MY_MACROS_HPP

// TODO Calibrate Sensor
// TODO try...catch... for checking if all arguments to all functions are valid
// TODO Other topologies besides square
// TODO MPI should be non-blocking
// TODO more than one isolated nodes (done?, test on the system)-> faulty on paths -> visualize paths on hardware
// TODO computational faults on reallocators
// TODO integrate with X-plane
// TODO fault detection system