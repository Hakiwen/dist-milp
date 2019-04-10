#include "NOC.hpp"
#include "NOC_CPLEX.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_APP.hpp"
#include "NOC_MPI.hpp"
#include "DIST_MILP_SOLVER.hpp"

ILOSTLBEGIN
typedef IloArray<IloNumVarArray> IloNumVarArray2D;

static void populatebyrow(IloModel model, IloNumVarArray x, IloRangeArray c);

int main (int argc, char* argv[]) // TODO try...catch... for checking if all arguments to all functions are valid
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
    void (*app_ptr[N_apps])(int);
    app_ptr[0] = &APP_LED;
    app_ptr[1] = &APP_LED;
    app_ptr[2] = &APP_LED;

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
     * NoC MPI Object
     */
    NOC_MPI NoC_MPI = NOC_MPI();

    /*
     * Solver Object
     */
    DIST_MILP_SOLVER prob = DIST_MILP_SOLVER("NoC.lp", "sol.xml");

    /*
     * Main Loop
     */
    while (1)
    {
        if (NoC_MPI.world_rank == 0)
        {
            do {
                NoC_CPLEX.write_LP(&NoC);
                if (prob.solve() != CPX_STAT_INFEASIBLE)
                {
                    NoC_CPLEX.read_Sol(&NoC, "sol.xml");
                    NoC.Disp();
                }
                else
                    {
                    NoC.solver_status = 0;
                }
            }
            while (NoC.solver_status && NoC_Fault.Fault_Detection(&NoC));
        }
        else
        {
            if(NoC.prev_app_to_run != NoC.app_to_run)
            {
                NoC.prev_app_to_run = NoC.app_to_run;
                app_ptr[0](NoC.app_to_run);
            }
        }
    }

    NoC_MPI.Finalize();
    while (1){}; // does nothing, but smiling at you :)

    return 0;

}  // END main