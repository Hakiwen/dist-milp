//
// Created by tkhamvilai on 4/4/19.
//

#ifndef DIST_MILP_NOC_HPP
#define DIST_MILP_NOC_HPP

#include "MY_MACROS.hpp"
#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <bitset>

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
    int *N_nodes_apps; // # of nodes corresponds to a given app
    int *N_links_apps; // # of links corresponds to a given app
    int N_nodes;
    int N_links;
    int N_neighbors;

    Eigen::MatrixXi G; // Incident matrix of the network
    Eigen::MatrixXi H; // Incident matrix of the application
    Eigen::MatrixXi A; // Adjacency matrix of the network
    Eigen::MatrixXi D; // Degree matrix of the network
    Eigen::MatrixXi L; // Laplacian matrix of the network

    Eigen::MatrixXi X_CRs_nodes;
    Eigen::MatrixXi X_paths_links;
    Eigen::MatrixXi R_apps;
    Eigen::MatrixXi M_apps;
    std::vector<Eigen::MatrixXi> X_comm_paths;
    Eigen::MatrixXi X_CRs_nodes_old;

    int *nodes_on_CRs; // node to run on each CR, solver sends
    Eigen::MatrixXi nodes_on_CRs_received; // node to run on each CR, receives from multiple allocators
    int *apps_on_CRs; // app to run on each CR
    int node_to_run; // node to run on each CR, each node receives
    int app_to_run; // app to run on each CR
    int *comm_path_to_use; //communication path to use for communicating from the reallocators to others

    int N_Faults_CR;
    int prev_N_Faults_CR;
    int *Fault_CRs; // faulty status of each node, solver receives
    int *Fault_Internal_CRs;
    int fault_internal_status_CR; // faulty status of each node, each node sends
    int *Fault_External_CRs;

    int N_Faults_Paths;
    int prev_N_Faults_Paths;
    int *Fault_Paths; // faulty status of each path, solver receives
    int *fault_status_Paths; // faulty status of neigbor paths, each node sends

    int solver_status;
    int obj_val;
    int var_size;
    int con_size;

    int *app_color;
    int allocator_app_ind;
    int allocator_app_num;

    NOC(int N_Row_CRs, int N_Col_CRs, int N_apps, int N_Row_apps[], int N_Col_apps[], int app_color[], int allocator_app_ind, int allocator_app_num);

    void CreateTopology(const char* topo);
    void CreateSquareTopology();
    void CreateAuxMatrices(const char* topo);
    Eigen::MatrixXi CreateIncidentMatrixSquareTopology(int N_Row, int N_Col);
    void CreateNeighborMatrixSquareTopology();
    void CreateDecisionMatrices();

    int get_app_from_node(int node);
    int get_app_from_link(int node);

    void Update_State();
    void Clear_State();

    int norm_of_difference(int i, int j);
    void App_Voter(int rank, int step);
    void Disp();
};

#endif //DIST_MILP_NOC_HPP
