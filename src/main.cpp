#include <ilcplex/ilocplex.h>
#include <ilconcert/iloexpression.h>
#include <ilcplex/ilocplexi.h>

#include "NOC.hpp"
#include "NOC_CPLEX.hpp"
#include "NOC_FAULT.hpp"
#include "DIST_MILP_SOLVER.hpp"

#include <mpi.h>

ILOSTLBEGIN
typedef IloArray<IloNumVarArray> IloNumVarArray2D;

static void populatebyrow(IloModel model, IloNumVarArray x, IloRangeArray c);

int main (void)
{
    /*
     * user-initialized topology
     */
    int N_Row_CRs = 4, N_Col_CRs = 4;
    int N_apps = 3;
    int N_Row_apps[N_apps], N_Col_apps[N_apps];
    N_Row_apps[0] = 2;    N_Col_apps[0] = 3;
    N_Row_apps[1] = 2;    N_Col_apps[1] = 2;
    N_Row_apps[2] = 2;    N_Col_apps[2] = 1;

    /*
     * NoC Object
     */
    NOC NoC = NOC(N_Row_CRs, N_Col_CRs, N_apps, N_Row_apps, N_Col_apps);
    NoC.CreateTopology("square");

    /*
     * NoC to CPLEX Object
     */
    NOC_CPLEX NoC_CPLEX = NOC_CPLEX();

    /*
     * NoC Fault Detection Object
     */
    NOC_FAULT NoC_Fault = NOC_FAULT();

    /*
     * Solver Object
     */
    DIST_MILP_SOLVER prob = DIST_MILP_SOLVER("NoC.lp", "sol.xml");

    /*
     * Main Loop
     */
//    do
//    {
//        NoC_CPLEX.write_LP(&NoC);
//        if(prob.solve() != CPX_STAT_INFEASIBLE)
//        {
//            NoC_CPLEX.read_Sol(&NoC, "sol.xml");
//            NoC.Disp();
//        }
//        else
//        {
//            NoC.status = 0;
//        }
//    }
//    while(NoC.status && NoC_Fault.Fault_Detection(&NoC));

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();

    while (1){}; // does nothing, but smiling at you :)

    return 0;

}  // END main