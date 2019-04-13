#include <unistd.h>
#include <signal.h>
#include <fstream>

#include "MY_MACROS.hpp"

#include "DIST_MILP_SOLVER.hpp"
#include "NOC.hpp"
#include "NOC_FAULT.hpp"
#include "NOC_APP.hpp"
#include "NOC_MPI.hpp"

#if defined(CPLEX_AS_SOLVER)
#include "NOC_CPLEX.hpp"
#include "CPLEX_SOLVER.hpp"
#elif defined(GLPK_AS_SOLVER)
#include "NOC_GLPK.hpp"
#include "GLPK_SOLVER.hpp"
#endif

using namespace std;

void sighandler(int sig)
{
    APP_LED(-1);
    exit(1);
}

int main (int argc, char* argv[]) // TODO try...catch... for checking if all arguments to all functions are valid
{
#ifndef __x86_64__
    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler);
    wiringPiSetup () ;
#endif

    /** User-Initialized Parameters **/

    int N_Row_CRs = 3, N_Col_CRs = 4;
    int N_apps = 3;
    int N_Row_apps[N_apps], N_Col_apps[N_apps];
    N_Row_apps[0] = 2;    N_Col_apps[0] = 2;
    N_Row_apps[1] = 1;    N_Col_apps[1] = 2;
    N_Row_apps[2] = 2;    N_Col_apps[2] = 1;
    void (*app_ptr[N_apps])(int);
    app_ptr[0] = &APP_LED;
    app_ptr[1] = &APP_LED;
    app_ptr[2] = &APP_LED;

    /** End of User Initialization **/

    NOC NoC = NOC(N_Row_CRs, N_Col_CRs, N_apps, N_Row_apps, N_Col_apps); // NoC Object
    NoC.CreateTopology("square");
    NOC_MPI NoC_MPI = NOC_MPI(); // NoC MPI Object
    NOC_FAULT NoC_Fault = NOC_FAULT(&NoC, NoC_MPI.world_rank); // NoC Fault Detection Object
#if defined(CPLEX_AS_SOLVER)
    NOC_CPLEX NoC_CPLEX = NOC_CPLEX(); // NoC to CPLEX Object
    CPLEX_SOLVER prob_CPLEX = CPLEX_SOLVER("NoC.lp", "sol.xml"); // Solver Object
#elif defined(GLPK_AS_SOLVER)
    NOC_GLPK NoC_GLPK = NOC_GLPK(); // NoC to GLPK Object
    GLPK_SOLVER prob_GLPK = GLPK_SOLVER("NoC.lp", "sol.txt"); // Solver Object
#endif

    /*
     * Main Loop
     */
    while (1)
    {
        if (NoC_MPI.world_rank == 0) // central node
        {
            if (NoC_Fault.Fault_Detection(&NoC, NoC_MPI.world_rank) && NoC.solver_status) {
#if defined(CPLEX_AS_SOLVER)
                NoC_CPLEX.write_LP(&NoC);
                if (prob_CPLEX.solve() != CPX_STAT_INFEASIBLE)
                {
                    NoC_CPLEX.read_Sol(&NoC, "sol.xml");
                }
#elif defined(GLPK_AS_SOLVER)
                NoC_GLPK.write_LP(&NoC);
                if(prob_GLPK.solve(&NoC_GLPK) == GLP_INTEGER_OPTIMAL)
                {
                    NoC_GLPK.read_Sol(&NoC);
                }
#endif
                else
                {
                    cout << "Infeasible Solution" << endl;
                    NoC.solver_status = 0;
                }
                NoC.Update_State();
            }
            NoC.Disp();
        }
        else if (NoC_MPI.world_rank == (NoC_MPI.world_size - 1)) // jet engine node (the last one)
        {
//            cout << "I'm the jet engine!" << endl;
        }
        else // computer resource node
        {
            NoC_Fault.Fault_Detection(&NoC, NoC_MPI.world_rank);
//            cout << "My Rank: " << NoC_MPI.world_rank << ", My Fault: " << NoC.fault_status << ", My App: ";
            app_ptr[0](NoC.app_to_run);
        }
        NoC_MPI.Scatter_Apps(&NoC); // TODO should be non-blocking
        NoC_MPI.Gather_Faults(&NoC);
        sleep(1);
    }

    while (1){}; // does nothing, but smiling at you :)

    return 0;

}  // END main