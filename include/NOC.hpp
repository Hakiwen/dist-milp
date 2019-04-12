//
// Created by tkhamvilai on 4/4/19.
//

#ifndef DIST_MILP_NOC_HPP
#define DIST_MILP_NOC_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <Eigen/Dense>

#define VERBOSE 0

class NOC{
public:
    int N_Row_CRs;
    int N_Col_CRs;
    int N_apps;
    int *N_Row_apps;
    int *N_Col_apps;

    int N_CRs;
    int N_paths;
    int *N_nodes_apps;
    int *N_links_apps;
    int N_nodes;
    int N_links;

    Eigen::MatrixXi G;
    Eigen::MatrixXi A;

    Eigen::MatrixXi X_CRs_nodes;
    Eigen::MatrixXi X_paths_links;
    Eigen::MatrixXi R_apps;
    Eigen::MatrixXi M_apps;
    Eigen::MatrixXi X_CRs_nodes_old;

    int *nodes_on_CRs; // node to run on each CR
    int *apps_on_CRs; // app to run on each CR, solver sends
    int app_to_run; // app to run on each CR, each node receives
    int prev_app_to_run;
    void (*app_ptr)(int);

    int N_Faults;
    int prev_N_Faults;
    int *Fault_CRs; // faulty status of each node, solver receives
    int fault_status; // faulty status of each node, each node sends
    int solver_status;

    int obj_val;
    int var_size;
    int con_size;

    NOC(int N_Row_CRs, int N_Col_CRs, int N_apps, int N_Row_apps[], int N_Col_apps[]);

    void CreateTopology(const char* topo);
    void CreateSquareTopology();
    void CreateIncidentMatrices(const char* topo);
    Eigen::MatrixXi CreateIncidentMatrixSquareTopology(int N_Row, int N_Col);
    void CreateDecisionMatrices();
    void InitialAllocation();
    int get_app_from_node(int node);
    int get_app_from_link(int node);
    void Update_State();
    void Disp();
};

#endif //DIST_MILP_NOC_HPP
