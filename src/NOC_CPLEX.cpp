//
// Created by tkhamvilai on 4/4/19.
//

#include "NOC_CPLEX.hpp"

NOC_CPLEX::NOC_CPLEX()
{

}

void NOC_CPLEX::write_LP(NOC *NoC)
{
    IloEnv env;
    try
    {
        IloModel model(env);
        CreateModel(model, NoC);

        cplex = IloCplex(model);
        cplex.exportModel("NoC.lp");
        cplex.clear();
    }
    catch (IloException& e)
    {
        std::cerr << "Concert exception caught: " << e << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught" << std::endl;
    }
//    env.end();
}

void NOC_CPLEX::CreateModel(IloModel model, NOC *NoC)
{
    IloEnv env = model.getEnv();

    /*
     * Create Decision Variables
     * */

    IloNumVarArray2D X_CRs_nodes(env, NoC->N_CRs), X_paths_links(env, NoC->N_paths);
    IloNumVarArray R_apps(env), M_apps(env);

    for (int i = 0; i < NoC->N_CRs; i++)
    {
        X_CRs_nodes[i] = IloNumVarArray(env);
        for (int j = 0; j < NoC->N_nodes; j++)
        {
            X_CRs_nodes[i].add(IloNumVar(env, 0.0, 1.0, ILOINT));
        }
        std::string name = "X_CRs_nodes_" + std::to_string(i);
        X_CRs_nodes[i].setNames(name.c_str());

        NoC->var_size += X_CRs_nodes[i].getSize();
    }

    for (int i = 0; i < NoC->N_paths; i++)
    {
        X_paths_links[i] = IloNumVarArray(env);
        for (int j = 0; j < NoC->N_links; j++) {
            X_paths_links[i].add(IloNumVar(env, 0.0, 1.0, ILOINT));
        }
        std::string name = "X_paths_links_" + std::to_string(i);
        X_paths_links[i].setNames(name.c_str());

        NoC->var_size += X_paths_links[i].getSize();
    }

    for (int i = 0; i < NoC->N_apps; i++)
    {
        R_apps.add(IloNumVar(env, 0.0, 1.0, ILOINT));
    }
    R_apps.setNames("R_apps");
    NoC->var_size += R_apps.getSize();

    for (int i = 0; i < NoC->N_nodes; i++)
    {
        M_apps.add(IloNumVar(env, 0.0, 1.0, ILOINT));
    }
    M_apps.setNames("M_apps");
    NoC->var_size += M_apps.getSize();


    /*
     * Create an Objective Function
     * */

    IloExpr obj(env);

    for (int i = 0; i < NoC->N_apps; i++)
    {
        obj.operator+=((NoC->N_nodes + 1)*R_apps[i]);
    }

    obj.operator-=(IloSum(M_apps));

    model.add(IloMaximize(env, obj));

    /*
     * Create Constraints
     * */

    IloRangeArray c(env);
    IloExpr constraints(env);

    // Resource allocation and partitioning
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        for (int j = 0; j < NoC->N_nodes; j++)
        {
            constraints.operator+=(X_CRs_nodes[i][j]);
        }
        if(NoC->Fault_CRs[i] == 0)
        {
            c.add(constraints <= 1);
        }
        else
        {
            c.add(constraints == 0);
        }
        constraints.clear();
    }

    for (int i = 0; i < NoC->N_nodes; i++)
    {
        for (int j = 0; j < NoC->N_CRs; j++)
        {
            constraints.operator+=(X_CRs_nodes[j][i]);
        }
        constraints.operator-=(R_apps[NoC->get_app_from_node(i+1) - 1]);
        c.add(constraints == 0);
        constraints.clear();
    }

    for (int i = 0; i < NoC->N_paths; i++)
    {
        for (int j = 0; j < NoC->N_links; j++)
        {
            constraints.operator+=(X_paths_links[i][j]);
        }
        c.add(constraints <= 1);
        constraints.clear();
    }

    for (int i = 0; i < NoC->N_links; i++)
    {
        for (int j = 0; j < NoC->N_paths; j++)
        {
            constraints.operator+=(X_paths_links[j][i]);
        }
        constraints.operator-=(R_apps[NoC->get_app_from_link(i+1) - 1]);
        c.add(constraints == 0);
        constraints.clear();
    }

    // Conformity to the architecture
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        for (int j = 0; j < NoC->N_links; j++)
        {
            for (int k = 0; k < NoC->N_nodes; k++)
            {
                constraints.operator+=(X_CRs_nodes[i][k]*NoC->A(k,j));
            }
            for (int k = 0; k < NoC->N_paths; k++)
            {
                constraints.operator-=(NoC->G(i,k)*X_paths_links[k][j]);
            }
            c.add(constraints == 0);
            constraints.clear();
        }
    }

    // Reallocating several applications
    for (int i = 0; i < NoC->N_CRs; i++)
    {
        for (int j = 0; j < NoC->N_nodes; j++)
        {
            if(NoC->X_CRs_nodes_old(i,j) == 1)
            {
                constraints.operator+=(1 - R_apps[NoC->get_app_from_node(j+1) - 1]);
                constraints.operator+=(M_apps[j] + X_CRs_nodes[i][j]);
                if (NoC->N_Faults == 0)
                {
                    c.add(constraints >= NoC->X_CRs_nodes_old(i, j)); // first allocation
                }
                else
                {
                    c.add(constraints == NoC->X_CRs_nodes_old(i, j));
                }
                constraints.clear();
            }
        }
    }

    // Priorities
    for (int i = 0; i < NoC->N_apps; i++)
    {
        if(i == 0)
        {
            c.add(R_apps[i] >= 1);
        }
        else
        {
            c.add(R_apps[i-1] - R_apps[i] >= 0);
        }
    }

    // Spatial Orientation
    int sum_N_nodes_apps = 0;
    int spatial_nodes_ind[NoC->N_apps][3]; // indices of the interested spatial nodes
    for (int k = 0; k < NoC->N_apps; k++)
    {
        spatial_nodes_ind[k][0] = sum_N_nodes_apps; // top left
        spatial_nodes_ind[k][1] = sum_N_nodes_apps + 1; // node to the right of the top left one
        spatial_nodes_ind[k][2] = sum_N_nodes_apps + NoC->N_Col_apps[k] ; // node below the top left one

        sum_N_nodes_apps += NoC->N_nodes_apps[k];

        for (int i = 0; i < NoC->N_CRs-1; i++)
        {
            try
            {
                if((i + 1) % NoC->N_Col_CRs != 0 && NoC->N_Col_apps[k] > 1) {
                    constraints.operator+=(X_CRs_nodes[i][spatial_nodes_ind[k][0]] -
                                           X_CRs_nodes[i + 1][spatial_nodes_ind[k][1]]); // top and right
                    c.add(constraints == 0);
                }
            }
            catch(...) {}
            constraints.clear();

            try
            {
                if(i < (NoC->N_CRs - NoC->N_Col_CRs) && NoC->N_Row_apps[k] > 1)
                {
                    constraints.operator+=(X_CRs_nodes[i][spatial_nodes_ind[k][0]] -
                                           X_CRs_nodes[i + NoC->N_Col_CRs][spatial_nodes_ind[k][2]]); // top and below
                    c.add(constraints == 0);
                }
            }
            catch(...) {}
            constraints.clear();
        }
    }

    NoC->con_size = c.getSize();

    model.add(c);
}

int NOC_CPLEX::read_Sol(NOC *NoC, const char* Sol_file)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(Sol_file);
    if (!result)
        return -1;

    int* var = new int[NoC->var_size];
    int* slack = new int[NoC->con_size];

    for (pugi::xml_node variable: doc.child("CPLEXSolutions").child("CPLEXSolution").child("variables").children("variable"))
    {
        var[variable.attribute("index").as_int()] = variable.attribute("value").as_int();
    }

    for (pugi::xml_node variable: doc.child("CPLEXSolutions").child("CPLEXSolution").child("linearConstraints").children("constraint"))
    {
        slack[variable.attribute("index").as_int()] = variable.attribute("slack").as_int();
    }

    int k = 0;
    for (k = 0; k < (NoC->N_apps + NoC->N_nodes); k++) {
        if (k < NoC->N_apps)
        {
            NoC->R_apps(k) = var[k];
        }
        else if (k >= NoC->N_apps && k < (NoC->N_apps + NoC->N_nodes))
        {
            NoC->M_apps(k - NoC->N_apps) = var[k];
        }
    }

    for (int i = 0; i < NoC->N_CRs; i++)
    {
        for (int j = 0; j < NoC->N_nodes; j++)
        {
            NoC->X_CRs_nodes(i,j) = var[k];
            k++;
        }
    }
    NoC->X_CRs_nodes_old = NoC->X_CRs_nodes;

    for (int i = 0; i < NoC->N_paths; i++)
    {
        for (int j = 0; j < NoC->N_links; j++)
        {
            NoC->X_paths_links(i,j) = var[k];
            k++;
        }
    }
}