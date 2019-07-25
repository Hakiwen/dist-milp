//
// Created by tkhamvilai on 4/4/19.
//
#include "NOC.hpp"

NOC::NOC(int N_Row_CRs, int N_Col_CRs, int N_apps, int N_Row_apps[], int N_Col_apps[], int app_color[], int allocator_app_ind, int allocator_app_num)
{
    // From user's inputs
    this->N_Row_CRs = N_Row_CRs;
    this->N_Col_CRs = N_Col_CRs;
    this->N_apps = N_apps;
    this->N_Row_apps = new int[N_apps];
    this->N_Row_apps = N_Row_apps;
    this->N_Col_apps = new int[N_apps];
    this->N_Col_apps = N_Col_apps;

    // Optimization
    this->obj_val = 0;
    this->var_size = 0;
    this->con_size = 0;

    // Total Number of Computer Resources and Nodes
    this->N_CRs = this->N_Row_CRs * this->N_Col_CRs;
    this->N_nodes = 0;
    this->N_nodes_apps = new int[this->N_apps];
    for (int i = 0; i < this->N_apps; i++)
    {
        this->N_nodes_apps[i] = this->N_Row_apps[i] * this->N_Col_apps[i];
        this->N_nodes += this->N_nodes_apps[i];
    }

    // Node/App to Run on Each CR
    this->nodes_on_CRs = new int[this->N_CRs];
    this->apps_on_CRs = new int[this->N_CRs];
    for (int i = 0; i < this->N_CRs; i++)
    {
        this->nodes_on_CRs[i] = 0; // initially alive, but not running any apps
        this->apps_on_CRs[i] = 0;
    }
    this->node_to_run = 0; // initially alive, but not running any apps
    this->app_to_run = 0; // initially alive, but not running any apps

    // Fault Detection on CRs
    this->N_Faults_CR = -1;
    this->prev_N_Faults_CR = -1;
    this->Fault_CRs = new int[this->N_CRs]; // 0 no fault, 1 has fault (for solver)
    this->Fault_Internal_CRs = new int[this->N_CRs];
    this->Fault_External_CRs = new int[this->N_CRs];
    this->Fault_Isolated_CRs = new int[this->N_CRs];
    for (int i = 0; i < this->N_CRs; i++)
    {
        this->Fault_CRs[i] = 0;
        this->Fault_Internal_CRs[i] = 0;
        this->Fault_External_CRs[i] = 0;
        this->Fault_Isolated_CRs[i] = 0;
    }
    this->fault_internal_status_CR = 0; // 0 no fault, 1 has fault (for each node)

    this->solver_status = 1; // 1 feasible, 0 infeasible

    // Color assigned to each app
    this->app_color = new int[N_apps];
    this->app_color = app_color;

    // Allocator
    this->allocator_app_ind = allocator_app_ind;
    this->allocator_app_num = allocator_app_num;
    for (int i = 0; i < this->allocator_app_num; i++)
    {
        this->allocator_nodes_ind.push_back(this->get_last_node_from_app(this->allocator_app_ind + i));
    }
    this->nodes_on_CRs_received.resize(this->N_CRs, this->allocator_app_num);
}

void NOC::CreateTopology(const char *topo)
{
    // Only Square Topology for NOW!
    this->CreateSquareTopology();
    this->CreateAuxMatrices(topo);
    this->CreateDecisionMatrices();
}

void NOC::CreateSquareTopology()
{
    this->N_neighbors = MAX_NEIGHBOR_NUM_SQUARE_TOPO; // UP, DOWN, LEFT, RIGHT
    this->N_paths =
            (this->N_Col_CRs - 1) * this->N_Row_CRs + (this->N_Row_CRs - 1) * this->N_Col_CRs; // square topology
    this->N_links_apps = new int[this->N_apps];
    this->N_links = 0;
    for (int i = 0; i < this->N_apps; i++)
    {
        this->N_links_apps[i] = (this->N_Col_apps[i] - 1) * this->N_Row_apps[i] + (this->N_Row_apps[i] - 1) * this->N_Col_apps[i];
        this->N_links += this->N_links_apps[i];
    }

    if (VERBOSE)
    {
        std::cout << "N_CRs: " << this->N_CRs << std::endl;
        std::cout << "N_paths: " << this->N_paths << std::endl;
        std::cout << "N_nodes: " << this->N_nodes << std::endl;
        std::cout << "N_links: " << this->N_links << std::endl;
    }
}

void NOC::CreateAuxMatrices(const char* topo){

    /* construct Degree Matrix and Adjacency*/
    this->CreateNeighborMatrixSquareTopology();
    if(VERBOSE)
    {
        std::cout << "D: \n" << this->D << std::endl;
        std::cout << "A: \n" << this->A << std::endl;
    }

    /* construct G matrix */
    this->G = this->CreateIncidentMatrixSquareTopology(this->N_Row_CRs, this->N_Col_CRs);
    if(VERBOSE)
    {
        std::cout << "G: \n" << this->G << std::endl;
    }

    /* construct H matrix */
    this->H.resize(this->N_nodes, this->N_links);
    int sum_N_Row_apps = 0, sum_N_Col_apps = 0;
    for (int k = 0; k < this->N_apps; k++)
    {
        Eigen::MatrixXi H_apps = this->CreateIncidentMatrixSquareTopology(this->N_Row_apps[k], this->N_Col_apps[k]);
        for (int i = sum_N_Row_apps; i < sum_N_Row_apps + this->N_nodes_apps[k]; i++)
        {
            for (int j = sum_N_Col_apps; j < sum_N_Col_apps + this->N_links_apps[k]; j++)
            {
                this->H(i,j) = H_apps(i-sum_N_Row_apps,j-sum_N_Col_apps);
            }
        }
        sum_N_Row_apps += this->N_nodes_apps[k];
        sum_N_Col_apps += this->N_links_apps[k];

        if(VERBOSE)
        {
            std::cout << "H_apps " << k+1 << ": \n" << H_apps << std::endl;
        }
    }
    if(VERBOSE)
    {
        std::cout << "H: \n" << this->H << std::endl;
    }

    /* Other variables for communication paths and Fault Detection on Paths*/
    this->comm_path_to_use = new int[this->N_paths];
    this->comm_path_to_use_received.resize(this->N_paths, this->allocator_app_num);
    this->path_to_run = new int[this->N_paths];
    this->N_Faults_Paths = -1;
    this->prev_N_Faults_Paths = -1;
    this->Fault_Paths = new int[this->N_paths]; // 0 no fault, 1 has fault (for solver)
    this->Fault_Paths_receive = new int[this->N_paths];
    this->Fault_Paths_send = new int[this->N_paths];
    for (int i = 0; i < this->N_paths; i++)
    {
        this->comm_path_to_use[i] = 0;
        this->path_to_run[i] = 0;
        this->Fault_Paths[i] = 0;
        this->Fault_Paths_receive[i] = 0;
        this->Fault_Paths_send[i] = -1;
    }
}

void NOC::CreateNeighborMatrixSquareTopology()
{
    int Neighbor_NUM = this->N_neighbors; // UP, DOWN, LEFT, RIGHT
    int neighbor_ind[Neighbor_NUM], UP_IND = 0, DOWN_IND = 1, LEFT_IND = 2, RIGHT_IND = 3;

    this->D = Eigen::MatrixXi::Zero(this->N_CRs, this->N_CRs);
    this->A = Eigen::MatrixXi::Identity(this->N_CRs, this->N_CRs); // allow connecting to itself

    for (int i = 0; i < this->N_CRs; i++)
    {
        neighbor_ind[LEFT_IND] = (i + 1) - 1;
        neighbor_ind[RIGHT_IND] = (i + 1) + 1;
        neighbor_ind[UP_IND] = (i + 1) - this->N_Col_CRs;
        neighbor_ind[DOWN_IND] = (i + 1) + this->N_Col_CRs;

        for (int j = 0; j < Neighbor_NUM; j++)
        {
            if (neighbor_ind[j] > 0 && neighbor_ind[j] <= this->N_CRs)
            {
                if ((this->Fault_Internal_CRs[neighbor_ind[j] - 1] || this->Fault_External_CRs[neighbor_ind[j] - 1]) == 0 && (this->Fault_Internal_CRs[i] || this->Fault_External_CRs[i]) == 0)
                {
                    this->D(i, i) += 1;
                    this->A(i, neighbor_ind[j] - 1) = 1;
                }
            }
        }

        if (this->N_Col_CRs > 1)
        {
            if ((i + 1) % this->N_Col_CRs == 1) // first column
            {
                if (neighbor_ind[LEFT_IND] > 0 && neighbor_ind[LEFT_IND] <= this->N_CRs)
                {
                    this->D(i, i) -= 1;
                    if ((this->Fault_Internal_CRs[neighbor_ind[LEFT_IND] - 1] || this->Fault_External_CRs[neighbor_ind[LEFT_IND] - 1]) == 1)
                    {
                        this->D(i, i) += 1;
                    }
                    this->A(i, neighbor_ind[LEFT_IND] - 1) = 0;
                }
            }
            else if ((i + 1) % this->N_Col_CRs == 0) // last column
            {
                if (neighbor_ind[RIGHT_IND] > 0 && neighbor_ind[RIGHT_IND] <= this->N_CRs)
                {
                    this->D(i, i) -= 1;
                    if ((this->Fault_Internal_CRs[neighbor_ind[RIGHT_IND] - 1] || this->Fault_External_CRs[neighbor_ind[RIGHT_IND] - 1]) == 1)
                    {
                        this->D(i, i) += 1;
                    }
                    this->A(i, neighbor_ind[RIGHT_IND] - 1) = 0;
                }
            }
        }

        if ((this->Fault_Internal_CRs[i] || this->Fault_External_CRs[i]) == 1)
        {
            this->D(i, i) = -1;
            this->A(i, i) = -1;
        }
    }
}

Eigen::MatrixXi NOC::CreateIncidentMatrixSquareTopology(int N_Row, int N_Col)
{
    int PATH_NUM = this->N_neighbors; // UP, DOWN, LEFT, RIGHT
    int path_ind[PATH_NUM], UP_IND = 0, DOWN_IND = 1, LEFT_IND = 2, RIGHT_IND = 3;

    int N_elements = N_Row * N_Col;
    int N_joint = (N_Col - 1)*N_Row + (N_Row - 1)*N_Col;
    Eigen::MatrixXi M(N_elements, N_joint);

    for (int i = 0; i < N_elements; i++)
    {
        int current_row = int(i / N_Col);
        path_ind[UP_IND] = (i+1) - N_Col + (N_Col - 1)*current_row;
        path_ind[DOWN_IND] = (i+1) + (N_Col - 1)*(current_row + 1);
        path_ind[LEFT_IND] = (i+1) - N_Col + (N_Col - 1)*(current_row + 1);
        path_ind[RIGHT_IND] = (i+1) + (N_Col - 1)*current_row;

        if(VERBOSE)
        {
            std::cout << "Node: " << i + 1
                      << ", up: " << path_ind[UP_IND]
                      << ", down: " << path_ind[DOWN_IND]
                      << ", left: " << path_ind[LEFT_IND]
                      << ", right: " << path_ind[RIGHT_IND] << std::endl;
        }

        for(int j = 0; j < PATH_NUM; j++)
        {
            if(path_ind[j] > 0 && path_ind[j] <= N_joint)
            {
                if (j == UP_IND || j == LEFT_IND)
                {
                    M(i, path_ind[j] - 1) = 1;
                }
                else if (j == DOWN_IND || j == RIGHT_IND)
                {
                    M(i, path_ind[j] - 1) = -1;
                }
            }
        }

        if(N_Col > 1)
        {
            if ((i + 1) % N_Col == 1) // first column
            {
                if (path_ind[LEFT_IND] > 0 && path_ind[LEFT_IND] <= N_joint)
                {
                    M(i, path_ind[LEFT_IND] - 1) = 0;
                }
            }
            else if ((i + 1) % N_Col == 0) // last column
            {
                if (path_ind[RIGHT_IND] > 0 && path_ind[RIGHT_IND] <= N_joint)
                {
                    M(i, path_ind[RIGHT_IND] - 1) = 0;
                }
            }
        }
    }
    return M;
}

void NOC::CreateDecisionMatrices()
{
    this->X_CRs_nodes.resize(this->N_CRs, this->N_nodes);
    this->X_paths_links.resize(this->N_paths, this->N_links);
    this->R_apps.resize(this->N_apps, 1);
    this->M_apps.resize(this->N_nodes, 1);
    Eigen::MatrixXi X_comm_paths_i;
    X_comm_paths_i.resize(this->N_paths, this->N_CRs);
    for (int i = 0; i < this->allocator_app_num; i++)
    {
        this->X_comm_paths.push_back(X_comm_paths_i);
    }

    this->X_CRs_nodes_old.resize(this->N_CRs, this->N_nodes);
    for (int i = 0; i < this->N_apps; i++)
    {
        this->R_apps(i) = 1;
    }

    if(VERBOSE)
    {
        std::cout << "X_CRs_nodes: \n" << this->X_CRs_nodes << std::endl;
        std::cout << "X_paths_links: \n" << this->X_paths_links << std::endl;
        std::cout << "R_apps: \n" << this->R_apps << std::endl;
        std::cout << "M_apps: \n" << this->M_apps << std::endl;
    }

    this->X_CRs_nodes_received.resize(this->N_CRs, this->N_nodes);
}

int NOC::get_last_node_from_app(int app)
{
    int sum_N_app = 0, node = 0;
    if(app == 0)
    {
        node = 0;
    }
    else if(app == -1)
    {
        node = -1;
    }
    else if(app <= this->N_apps)
    {
        for (int i = 0; i < app; i++)
        {
            sum_N_app += this->N_nodes_apps[i];
        }
        node = sum_N_app;
    }
    return node;
}

int NOC::get_app_from_node(int node)
{
    int sum_N_nodes = 0, apps = 0;
    for (int k = 0; k < this->N_apps; k++)
    {
        if(node == 0)
        {
            apps = 0;
        }
        else if(node == -1)
        {
            apps = -1;
        }
        else if((node-1) < this->N_nodes_apps[k] + sum_N_nodes)
        {
            apps = k+1;
            break;
        }
        else
        {
            sum_N_nodes += this->N_nodes_apps[k];
        }
    }
    return apps;
}

int NOC::get_app_from_link(int link)
{
    int sum_N_link = 0, apps = 0;
    for (int k = 0; k < this->N_apps; k++)
    {
        if(link < this->N_links_apps[k] + sum_N_link)
        {
            apps = k;
            break;
        }
        else
        {
            sum_N_link += this->N_links_apps[k];
        }
    }
    return apps;
}

Eigen::MatrixXi NOC::get_X_from_nodes(Eigen::MatrixXi nodes_on_CRs_received_voted, int N_rows, int N_cols)
{
    Eigen::MatrixXi X_matrix = Eigen::MatrixXi::Zero(N_rows, N_cols);

    for (int i = 0; i < N_rows; i++)
    {
        if(nodes_on_CRs_received_voted(i,0) >= 1 && nodes_on_CRs_received_voted(i,0) <= N_cols)
        {
            X_matrix(i, nodes_on_CRs_received_voted(i,0)-1) = 1;
        }
    }

    return X_matrix;
}

void NOC::Update_State()
{
    for (int i = 0; i < this->N_CRs; i++)
    {
        for (int j = 0; j < this->N_nodes; j++)
        {
            if (this->Fault_CRs[i] == 1) // that node is faulty
            {
                this->nodes_on_CRs[i] = -1;
                break;
            }
            else if (this->solver_status == 0) // infeasible solution, all alive nodes run nothing
            {
                this->nodes_on_CRs[i] = 0;
                break;
            }
            else if (this->X_CRs_nodes(i, j) == 1) // i_th CR runs j_th node
            {
                this->nodes_on_CRs[i] = j + 1;
                break;
            }
            else
            {
                this->nodes_on_CRs[i] = 0; //  i_th CR do not run j_th node
            }
        }
        this->apps_on_CRs[i] = get_app_from_node(this->nodes_on_CRs[i]);
    }

    for (int i = 0; i < this->N_paths; i++)
    {
        this->comm_path_to_use[i] = 0;
    }
    for (int k = 0; k < this->allocator_app_num; k++)
    {
        for (int i = 0; i < this->N_paths; i++)
        {
            int sum_path_in_CRs = 0;
            for (int j = 0; j < this->N_CRs; j++)
            {
                sum_path_in_CRs += abs(this->X_comm_paths[k](i, j));
            }
            if (this->Fault_Paths[i] == 1) // that path is faulty
            {
                this->comm_path_to_use[i] = -1;
            }
            else if (this->solver_status == 0) // infeasible solution, all alive paths run nothing
            {
                this->comm_path_to_use[i] = 0;
            }
            else if (sum_path_in_CRs > 0) // comm path is used, either -1 or 1
            {
                this->comm_path_to_use[i] |= (1 << k);
            }
            else
            {
                this->comm_path_to_use[i] |= (0 << k);
            }
        }
    }
}

void NOC::Clear_State()
{
    for (int i = 0; i < this->N_CRs; i++)
    {
        this->nodes_on_CRs[i] = 0;
    }
    for (int i = 0; i < this->N_paths; i++)
    {
        this->comm_path_to_use[i] = 0;
    }
}

void NOC::Voter(int rank, int step)
{
    this->App_Voter(rank, step); // vote on reallocator app signals
    this->Path_Voter(step); // vote on reallocator path signals
}

void NOC::App_Voter(int rank, int step)
{
    if(step == 0)
    {
        this->node_to_run = this->nodes_on_CRs_received(rank-1, 0);
    }
    else
    {
        int mismatch_1_2 = MathHelperFunctions_norm_of_difference(1, 2, this->nodes_on_CRs_received); // = 0 if values matches, != 0 otherwise
        int mismatch_2_3 = MathHelperFunctions_norm_of_difference(2, 3, this->nodes_on_CRs_received);
        int mismatch_3_1 = MathHelperFunctions_norm_of_difference(3, 1, this->nodes_on_CRs_received);

        if(mismatch_1_2 == 0) // values match
        {
            this->node_to_run = this->nodes_on_CRs_received(rank-1, 0); // allocators 1 and 2 agree, listen to one of them, here 1
            this->X_CRs_nodes_received = this->get_X_from_nodes(this->nodes_on_CRs_received.col(0), this->N_CRs, this->N_nodes);
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 1 and 2 match in app" << std::endl;
#endif
        }
        else if(mismatch_2_3 == 0)
        {
            this->node_to_run = this->nodes_on_CRs_received(rank-1, 1); // allocators 2 and 3 agree, listen to one of them, here 2
            this->X_CRs_nodes_received = this->get_X_from_nodes(this->nodes_on_CRs_received.col(1), this->N_CRs, this->N_nodes);
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 2 and 3 match in app" << std::endl;
#endif
        }
        else if(mismatch_3_1 == 0)
        {
            this->node_to_run = this->nodes_on_CRs_received(rank-1, 2); // allocators 3 and 1 agree, listen to one of them, here 3
            this->X_CRs_nodes_received = this->get_X_from_nodes(this->nodes_on_CRs_received.col(2), this->N_CRs, this->N_nodes);
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 3 and 1 match in app" << std::endl;
#endif
        }
        else if (mismatch_1_2 == -1 || mismatch_2_3 == -1 || mismatch_3_1 == -1)
        {
            bool is_alloc = false;
            if(mismatch_1_2 == -1)
            {
                for (int i = 0; i < this->allocator_app_num; i++)
                {
                    is_alloc = is_alloc || (this->nodes_on_CRs_received(rank-1, 2) == this->allocator_nodes_ind[i]);
                    if(is_alloc)
                    {
                        break;
                    }
                }

                if(is_alloc)
                {
                    this->node_to_run = this->get_last_node_from_app(this->allocator_app_ind);
                }
                else if(this->nodes_on_CRs_received(rank-1, 2) == -1)
                {
                    this->node_to_run = -1;
                }
            }
            else if(mismatch_2_3 == -1)
            {
                for (int i = 0; i < this->allocator_app_num; i++)
                {
                    is_alloc = is_alloc || (this->nodes_on_CRs_received(rank-1, 0) == this->allocator_nodes_ind[i]);
                    if(is_alloc)
                    {
                        break;
                    }
                }

                if(is_alloc)
                {
                    this->node_to_run = this->get_last_node_from_app(this->allocator_app_ind);
                }
                else if(this->nodes_on_CRs_received(rank-1, 0) == -1)
                {
                    this->node_to_run = -1;
                }
            }
            else if(mismatch_3_1 == -1)
            {
                for (int i = 0; i < this->allocator_app_num; i++)
                {
                    is_alloc = is_alloc || (this->nodes_on_CRs_received(rank-1, 1) == this->allocator_nodes_ind[i]);
                    if(is_alloc)
                    {
                        break;
                    }
                }

                if(is_alloc)
                {
                    this->node_to_run = this->get_last_node_from_app(this->allocator_app_ind);
                }
                else if(this->nodes_on_CRs_received(rank-1, 1) == -1)
                {
                    this->node_to_run = -1;
                }
            }

#if defined(__x86_64__)
            if (VERBOSE) std::cout << "only one allocator left, I will be another allocator app" << std::endl;
#endif
        }
        else // all allocators give different results
        {
            // trust no allocators, keep doing what's it doing
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "no allocators match in app" << std::endl;
#endif
        }
    }

    this->app_to_run = this->get_app_from_node(this->node_to_run); // mapping from node to app

    if(this->app_to_run >= this->allocator_app_ind && this->app_to_run < this->allocator_app_ind + this->allocator_app_num)
    {
        for (int i = 0; i < this->N_CRs; i++)
        {
            for (int j = 0; j < this->N_nodes; j++)
            {
                this->X_CRs_nodes_old(i, j) = 0;
            }
            if(this->nodes_on_CRs_received(i,0) > 0)
            {
                this->X_CRs_nodes_old(i, this->nodes_on_CRs_received(i, 0) - 1) = 1;
            }
        }
    }
}

void NOC::Path_Voter(int step)
{
    if(step == 0)
    {
        for (int i = 0; i < this->N_paths; i++)
        {
            this->path_to_run[i] = this->comm_path_to_use_received(i, 0);
        }
    }
    else
    {
        int mismatch_1_2 = MathHelperFunctions_norm_of_difference(1, 2, this->comm_path_to_use_received); // = 0 if values matches, != 0 otherwise
        int mismatch_2_3 = MathHelperFunctions_norm_of_difference(2, 3, this->comm_path_to_use_received);
        int mismatch_3_1 = MathHelperFunctions_norm_of_difference(3, 1, this->comm_path_to_use_received);

        if(mismatch_1_2 == 0) // values match
        {
            for (int i = 0; i < this->N_paths; i++)
            {
                this->path_to_run[i] = this->comm_path_to_use_received(i, 0); // allocators 1 and 2 agree, listen to one of them, here 1
            }
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 1 and 2 match in path" << std::endl;
#endif
        }
        else if(mismatch_2_3 == 0)
        {
            for (int i = 0; i < this->N_paths; i++)
            {
                this->path_to_run[i] = this->comm_path_to_use_received(i, 1); // allocators 2 and 3 agree, listen to one of them, here 2
            }
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 2 and 3 match in path" << std::endl;
#endif
        }
        else if(mismatch_3_1 == 0)
        {
            for (int i = 0; i < this->N_paths; i++)
            {
                this->path_to_run[i] = this->comm_path_to_use_received(i, 2); // allocators 3 and 1 agree, listen to one of them, here 3
            }
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "allocators 3 and 1 match in path" << std::endl;
#endif
        }
        else // all allocators give different results
        {
            // trust no allocators, keep doing what's it doing
#if defined(__x86_64__)
            if (VERBOSE) std::cout << "no allocators match in path" << std::endl;
#endif
        }
    }
}

void NOC::Disp()
{
    if(VERBOSE)
    {
        std::cout << "con: " << this->con_size << std::endl;
        std::cout << "var: " << this->var_size << std::endl;
        std::cout << "X_CRs_nodes: \n" << this->X_CRs_nodes << std::endl;
        std::cout << "X_paths_links: \n" << this->X_paths_links << std::endl;
        std::cout << "R_apps: \n" << this->R_apps << std::endl;
        std::cout << "M_apps: \n" << this->M_apps << std::endl;
    }

    Eigen::MatrixXi result1(this->N_Row_CRs, this->N_Col_CRs), result2(this->N_Row_CRs, this->N_Col_CRs);

    for (int i = 0; i < this->N_Row_CRs; i++)
    {
        for (int j = 0; j < this->N_Col_CRs; j++)
        {
            result1(i,j) = this->nodes_on_CRs[i*this->N_Col_CRs + j];
            result2(i,j) = this->apps_on_CRs[i*this->N_Col_CRs + j];
        }
    }

//    std::cout << "status: " << this->solver_status << std::endl;
    std::cout << "result1: \n" << result1 << std::endl;
    std::cout << "result2: \n" << result2 << std::endl;

    for(int i = 0; i < this->N_paths; i ++)
    {
        std::cout << "comm path " << i+1 << ": " << comm_path_to_use[i] << ", ";
        for (unsigned int j = (1 << (this->allocator_app_num - 1)); j > 0; j = j / 2)
        {
            ( (comm_path_to_use[i] & j) && (comm_path_to_use[i] >= 0) ) ? std::printf("1") : std::printf("0");
        }
        std::cout << std::endl;
    }

//    for (int k = 0; k < this->allocator_app_num; k++)
//    {
//        std::cout << "paths from allocator: " <<  k+1 << "\n" << this->X_comm_paths[k] << std::endl;
//    }
}