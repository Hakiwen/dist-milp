//
// Created by tkhamvilai on 4/4/19.
//

#ifndef DIST_MILP_NOC_HPP
#define DIST_MILP_NOC_HPP

#include <iostream>

#include <Eigen/Dense>

#define VERBOSE 1

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

    int N_Faults;
    int *Fault_CRs;

    int obj_val;
    int var_size;
    int con_size;

    int status;

    NOC(int N_Row_CRs, int N_Col_CRs, int N_apps, int N_Row_apps[], int N_Col_apps[]);

    void CreateTopology(const char* topo);
    void CreateSquareTopology();
    void CreateIncidentMatrices(const char* topo);
    Eigen::MatrixXi CreateIncidentMatrixSquareTopology(int N_Row, int N_Col);
    void CreateDecisionMatrices();
    void InitialAllocation();
    int get_app_from_node(int node);
    int get_app_from_link(int node);
    void Disp();
};

#endif //DIST_MILP_NOC_HPP
